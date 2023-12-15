#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // 클라이언트 소켓 생성
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        error("Error opening socket");

    // 서버 주소 초기화
    bzero((char *)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // 서버의 IP 주소
    serverAddr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error("Error on connecting");

    printf("Connected to the server.\n");

    // 전송할 파일 열기
    FILE *file = fopen("file_to_send.txt", "rb");
    if (!file)
        error("Error opening file");

    // 파일 전송
    ssize_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    fclose(file);
    close(clientSocket);

    printf("File sent successfully.\n");

    return 0;
}
