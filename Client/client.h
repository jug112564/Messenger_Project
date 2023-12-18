#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_NAME_BUF_SIZE 10
#define MAX_TEXT_BUF_SIZE 70
#define MAX_COMBINED_BUF_SIZE (MAX_NAME_BUF_SIZE + MAX_TEXT_BUF_SIZE)

extern int client_sock;
extern int file_sock;

extern char name_buf[MAX_NAME_BUF_SIZE];
//pthread_t send_thread, recv_thread;

void init_socket();
void connect_server(char* ip, int port);
void send_msg(char* buf);
void* recv_msg();

#endif