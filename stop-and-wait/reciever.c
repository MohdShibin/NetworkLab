#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_SEQ 2
#define INC_SEQ(k) k = (k + 1) % MAX_SEQ

void error(char *msg) {
    printf("%s\n", msg);
    exit(1);
}

struct Frame {
    int data;
    int seqno;
};

int receiving_seqno = 0;

int main(int argc, char **argv) {
    srand(time(0));

    if(argc < 2) {
        error("Format: ./file.out PORT");
    }

    char *ip_addr = "127.0.0.1";
    int port = atoi(argv[1]);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0) {
        error("Socket error");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_addr);
    address.sin_port = htons(port);

    if(bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Bind error");
    }

    if(listen(socket_fd, 3) < 0) {
        error("Listen error");
    }

    socklen_t addrlen = sizeof(address);
    int new_socket = accept(socket_fd, (struct sockaddr *)&address, &addrlen);
    if(new_socket < 0) {
        error("Accept error");
    }

    int expected_data = 1;
    while(1) {
        // RECEIVE FRAME
        struct Frame data_frame;
        if(recv(new_socket, &data_frame, sizeof(data_frame), 0) < 0) {
            error("Recv error");
        }
        if(data_frame.data == expected_data) {
            printf("Received Frame | Data: %d\n", data_frame.data);
            expected_data++;
        }else {
            printf("Received Duplicate Frame | Data: %d\n", data_frame.data);
        }

        // SEND ACK
        struct Frame ack_frame;

        if(rand() % 2 == 1) {
            if(send(new_socket, &data_frame, sizeof(data_frame), 0) < 0) {
                error("Error sending");
            }
        }
        printf("Ack Sent\n");
    }
} 