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

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        printf("Socket error\n");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_addr);
    address.sin_port = htons(port);

    if(connect(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        printf("Connect error\n");
        exit(1);
    }

    fd_set set;
    int max_fd = socket_fd;

    while(1) {
        FD_ZERO(&set);
        FD_SET(0, &set);
        FD_SET(socket_fd, &set);

        select(max_fd + 1, &set, NULL, NULL, NULL);

        if(FD_ISSET(socket_fd, &set)) {
            char buffer[1024];
            int len = recv(socket_fd, buffer, sizeof(buffer), 0);
            buffer[len] = '\0';
            printf("%s\n", buffer);
        }
        else {
            char buffer[1024];
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if(send(socket_fd, buffer, strlen(buffer), 0) < 0) {
                printf("Send error\n");
                exit(1);
            }
        }
    }
}