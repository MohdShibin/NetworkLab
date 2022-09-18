#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>

#define SIZE 1024

void error(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

int main(int argc, char **argv) {
    if(argc < 2) {
        error("Format: ./file.out PORT");
    }

    char *ip_addr = "127.0.0.1";
    int port = atoi(argv[1]);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0) {
        error("Socket error");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip_addr);

    if(bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Bind error");
    }

    socklen_t addrlen = sizeof(address);

    while(1) {
        time_t t = time(0);
        char buffer[SIZE];
        recvfrom(sock_fd, buffer, SIZE, 0, (struct sockaddr *)&address, &addrlen);
        printf("%s\n", ctime(&t));
        sendto(sock_fd, &t, sizeof(t), 0, (struct sockaddr *)&address, sizeof(address));
    }
}