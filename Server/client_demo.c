#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUF 80

int client_sock;
char name_buf[MAX_BUF];
pthread_t send_thread, recv_thread;

void* send_msg() {
    char buf[MAX_BUF];

    while (1) {
        memset(buf, 0, MAX_BUF);
        // Read a line from stdin
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            perror("fgets");
            exit(EXIT_FAILURE);
        }

        // Remove the newline character if present
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        // send
        if (send(client_sock, buf, strlen(buf), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

void* recv_msg() {
    char buf[MAX_BUF];
    int recv_len;
    while (1) {
        // recv
        memset(buf, 0, MAX_BUF);
        recv_len = recv(client_sock, buf, MAX_BUF, 0);

        //errir or server closed
        if (recv_len <= 0) {
            if (recv_len == 0) {
                printf("Server closed the connection\n");
            } else {
                perror("recv");
            }
            close(client_sock);
            exit(EXIT_SUCCESS);
        }

        printf("%s\n", buf);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;

    //input name
    printf("input your username for use chatting : ");
    memset(name_buf, 0, MAX_BUF);
    scanf("%s", name_buf);

    // socket create
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // send my name
    if (send(client_sock, name_buf, strlen(name_buf), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Create threads for sending and receiving messages
    if (pthread_create(&send_thread, NULL, send_msg, NULL) != 0) {
        perror("pthread_create for send");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&recv_thread, NULL, recv_msg, NULL) != 0) {
        perror("pthread_create for recv");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish (this won't happen in this example, threads will run indefinitely)
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    // close
    close(client_sock);
    return 0;
}
