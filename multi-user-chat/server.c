#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>

#define IP_ADDR "127.0.0.1"

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Format: ./file port\n");
        exit(1);
    }

    char *ip_addr = "127.0.0.1";
    int port = atoi(argv[1]);

    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(master_socket < 0) {
        printf("Socket error\n");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_addr);
    address.sin_port = htons(port);

    if(bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Bind error\n");
        exit(1);
    }

    if(listen(master_socket, 3) < 0) {
        printf("Listen error\n");
        exit(1);
    }


    fd_set set;
    int max_fd = master_socket;

    socklen_t addrlen = sizeof(address);
    int client_sockets[100], no_of_clients = 0;
    char **client_names;
    while(1) {
        FD_ZERO(&set);
        FD_SET(master_socket, &set);

        for(int i=0; i<no_of_clients; i++) {
            FD_SET(client_sockets[i], &set);
        }

        if(select(max_fd + 1, &set, NULL, NULL, NULL) < 0) {
            printf("Select error\n");
            exit(1);
        }

        if(FD_ISSET(master_socket, &set)) {
            printf("Connection on master socket\n");
            int new_socket = accept(master_socket, (struct sockaddr *)&address, &addrlen);
            client_sockets[no_of_clients++] = new_socket;
            if(new_socket > max_fd) {
                max_fd = new_socket;
            }
        }else {
            for(int i=0; i<no_of_clients; i++) {
                int sock_fd = client_sockets[i];
                if(FD_ISSET(sock_fd, &set)) {
                    char buffer[500];
                    int len = recv(sock_fd, buffer, sizeof(buffer), 0);
                    if(len < 0) {
                        printf("Recv error\n");
                        exit(1);
                    }
                    buffer[len] = '\0';
                    printf("%d: %s\n", sock_fd, buffer);

                    for(int j=0; j<no_of_clients; j++) {
                        if(i != j) {
                            printf("Sent to %d\n", client_sockets[j]);
                            if(send(client_sockets[j], buffer, strlen(buffer), 0) < 0) {
                                printf("Broadcast error\n");
                                exit(1);
                            }
                        }
                    }
                }
            }
        }
    }
}