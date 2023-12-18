#include "client.h"

void init_socket()
{
    // socket create
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    file_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (file_sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
}

void connect_server(char* ip, int port)
{
    struct sockaddr_in chat_server_addr,file_server_addr;
    memset(&chat_server_addr, 0, sizeof(chat_server_addr));
    chat_server_addr.sin_family = AF_INET;
    chat_server_addr.sin_port = htons(port);
    chat_server_addr.sin_addr.s_addr = inet_addr(ip);

    // connect
    if (connect(client_sock, (struct sockaddr*)&chat_server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    // send my name
    if (send(client_sock, name_buf, strlen(name_buf), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    memset(&chat_server_addr, 0, sizeof(chat_server_addr));
    chat_server_addr.sin_family = AF_INET;
    chat_server_addr.sin_port = htons(port+1);
    chat_server_addr.sin_addr.s_addr = inet_addr(ip);
    // connect
    if (connect(file_sock, (struct sockaddr*)&chat_server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}

void send_msg(char* buf) {
    if (send(client_sock, buf, strlen(buf), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
}

void send_file(char* file_name) {
    char buffer[1024];
    // 전송할 파일 열기
    FILE *file = fopen(file_name, "rb");
    if (!file)
        error("Error opening file");

    // 파일 전송
    ssize_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(file_sock, buffer, bytesRead, 0);
    }

    fclose(file);
}

void* recv_msg() {
    char buf[MAX_TEXT_BUF_SIZE];
    int recv_len;
    while (1) {
        // recv
        memset(buf, 0, MAX_TEXT_BUF_SIZE);
        recv_len = recv(client_sock, buf, MAX_TEXT_BUF_SIZE, 0);

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

       insert_text_received(buf);
    }
    return NULL;
}

