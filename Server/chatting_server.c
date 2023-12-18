#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

#define MAX_BUF 70
#define MAX_CLIENT 10

int client_count=0;
int client_socks[MAX_CLIENT];
char client_names[MAX_CLIENT][10];
int client_sock;

void send_all(char* msg,int msg_len)
{
    //send (echo)
    for (int i=0; i<client_count; i++)
    {
        write(client_socks[i], msg, msg_len); 
    }
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size = sizeof(struct sockaddr_in);
    char buf[MAX_BUF];
    struct epoll_event events[MAX_CLIENT];
    int epoll_fd, event_count;

    // socket create
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    // bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(server_sock, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event server_event;
    server_event.events = EPOLLIN;
    server_event.data.fd = server_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &server_event) == -1) {
        perror("epoll_ctl");
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        event_count = epoll_wait(epoll_fd, events, MAX_CLIENT, 1000);  // 1초마다 체크
        if (event_count == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < event_count; ++i) {
            if (events[i].data.fd == server_sock) {
            // Accept new connection
            client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &sin_size);
            if (client_sock == -1) {
                perror("accept");
                continue;
            }

            int bytes_received = recv(client_sock, client_names[client_count], sizeof(client_names[client_count]) - 1, 0);
            if (bytes_received <= 0) {
            perror("recv");
            close(client_sock);
            continue;
            }
                client_names[client_count][bytes_received] = '\0';
                char client_message[80];
                snprintf(client_message, sizeof(client_message), "New Client %s connected\n", client_names[client_count]);

                printf("%s", client_message);
                send_all(client_message, strlen(client_message));

                client_socks[client_count++] = client_sock;

                struct epoll_event client_event;
                client_event.events = EPOLLIN;
                client_event.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &client_event) == -1) {
                    perror("Epoll control failed for client");
                    close(client_sock);
                    continue;
                }
            } else {
                // 메시지 받기
                int client_index = -1;
                for (int j = 0; j < client_count; ++j) {
                    if (events[i].data.fd == client_socks[j]) {
                        client_index = j;
                        break;
                    }
                }

                if (client_index == -1) {
                    // 에러 처리
                    perror("Client index not found");
                    continue;
                }

                int bytes_received = recv(events[i].data.fd, buf, sizeof(buf) - 1, 0);
                if (bytes_received <= 0) {
                    // 클라이언트 종료
                   char client_message[80];
                    snprintf(client_message, sizeof(client_message), "Client %s disconnected\n", client_names[client_index]);

                    printf("%s", client_message);
                    send_all(client_message, strlen(client_message));
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                } else {
                    // 받은 거 출력
                    buf[bytes_received] = '\0';
                    printf("%s: %s\n", client_names[client_index], buf);

                    char client_message[80];
                    snprintf(client_message, sizeof(client_message), "%s: %s", client_names[client_index], buf);

                    // send (echo)
                    send_all(client_message, strlen(client_message));
                }
            }
        }
    }

    // 연결 종료
    close(server_sock);
    close(epoll_fd);

    return 0;
}
