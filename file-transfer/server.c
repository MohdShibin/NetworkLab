#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct pals{
    char palindromes[1024][1024];
    int palindromes_count;
};

void error(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

int is_palindrome(char *str) {
    int i=0;
    int j=strlen(str)-1;
    while(i<j) {
        if(str[i] != str[j]) {
            return 0;
        }
        i++;
        j--;
    }
    return 1;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        error("Format: ./file.out PORT");
    }
    
    char *ip_addr = "127.0.0.1";
    int port = atoi(argv[1]);

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        error("Socket error");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_addr);
    address.sin_port = htons(port);

    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Bind error");
    }

    if(listen(sock_fd, 1) < 0) {
        error("Listen error");
    }

    socklen_t addrlen = sizeof(address);
    int new_socket = accept(sock_fd, (struct sockaddr *)&address, &addrlen);

    while(1) {
        // RECIEVE FILE
        char buffer[1024];
        int len = recv(new_socket, buffer, sizeof(buffer), 0);
        if(len < 0) {
            error("Recv error");
        }
        buffer[len] = '\0';
        printf("%s\n", buffer);
        
        // BREAK IF EOF
        if(strcmp(buffer, "EOF") == 0) {
            break;
        }
    
        // FIND AND SEND PALINDROMES
        char str[1024];
        for(int i=0; i<strlen(buffer); i++) {
            if((buffer[i] >= 'a' && buffer[i] <= 'z') ||
               (buffer[i] >= 'A' && buffer[i] <= 'Z') ||
               (buffer[i] >= '0' && buffer[i] <= '9')) {
                strncat(str, &buffer[i], 1);
            }
            else if(strlen(str) > 0){
                if(is_palindrome(str)) {
                    if(send(new_socket, str, sizeof(str), 0) < 0) {
                        error("Send error\n");
                    }
                }
                bzero(str, 1024);
            }
        }

        // MARK END OF PALINDROMES
        char eop[1024] = "EOP";
        send(new_socket, eop, sizeof(eop), 0);
    }
}