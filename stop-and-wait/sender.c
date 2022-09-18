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

int data = 0;
int sending_seqno = 0;

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

    socklen_t addrlen = sizeof(address);
    if(connect(socket_fd, (struct sockaddr *)&address, addrlen) < 0) {
        error("Connect error");
    }

    int no_of_frames;
    printf("Enter no of frames: ");
    scanf("%d", &no_of_frames);

    fd_set set;
    int max_fd;

    int i=1;
    while(i <= no_of_frames) {
        FD_ZERO(&set);
        FD_SET(socket_fd, &set);
        max_fd = socket_fd;

        // SEND A FRAME
        struct Frame data_frame;
        data_frame.data = i;

        if(rand() % 2 == 1) {
            if(send(socket_fd, &data_frame, sizeof(data_frame), 0) < 0) {
                error("Error sending");
            }
        }
        printf("Sent Frame | Data: %d\n", data_frame.data);

        struct timeval timeout = {5, 0};
        select(max_fd + 1, &set, NULL, NULL, &timeout);

        if(FD_ISSET(socket_fd, &set)) {
            // RECEIVE ACK
            struct Frame ack_frame;
            if(recv(socket_fd, &ack_frame, sizeof(ack_frame), 0) < 0) {
                error("Recv error");
            }
            printf("Ack Received\n\n");

            i++;
        }else {
            printf("Timeout! Resending Frame\n\n");
        }
    }
}