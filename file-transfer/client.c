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

    socklen_t addrlen = sizeof(address);
    if(connect(sock_fd, (struct sockaddr *)&address, addrlen) < 0) {
        error("Connect error");
    }

    // SEND FILE
    FILE *fp = fopen("file1.txt", "r");
    FILE *fp2 = fopen("file2.txt", "w");
    while(!feof(fp)) {
        char buffer[1024];
        fgets(buffer, sizeof(buffer), fp);
        buffer[strcspn(buffer, "\n")] = '\0';
        printf("%s\n\n", buffer);
        send(sock_fd, buffer, sizeof(buffer), 0);

        // RECV PALINDROMES
        while(1) {
            char buffer[1024];
            int len = recv(sock_fd, buffer, sizeof(buffer), 0);
            if(len < 0) {
                error("Error recv");
            }
            if(strcmp(buffer, "EOP") == 0) {
                break;
            }
            fputs(buffer, fp2);
            fputc('\n', fp2);
        }
    }
    fclose(fp);

    // SEND EOF
    char eof[1024] = "EOF";
    send(sock_fd, eof, strlen(eof), 0);
}