#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 50000

void error_handling(char *message);

int main() {
    printf("\tServer starting...\n\tWaiting for connections...\n\n");

    int serv_sock;
    socklen_t clnt_adr_sz;

    struct sockaddr_in serv_adr, clnt_adr;

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1)
        error_handling("UDP socket creation error!\n");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(PORT);

    if (bind(serv_sock, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error!\n");

    while (1) {
        char message[BUF_SIZE];
        clnt_adr_sz = sizeof(clnt_adr);
        ssize_t str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr *) &clnt_adr, &clnt_adr_sz);
        if (str_len < 0)
            break;
        printf("Received from client: %s\n", message);
        sendto(serv_sock, message, str_len, 0, (struct sockaddr *) &clnt_adr, clnt_adr_sz);
        memset(message, 0, strlen(message));
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

