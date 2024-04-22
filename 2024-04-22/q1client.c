#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUF_SIZE 256
#define PORT 50000

void error_handling(char *message);

int main(int argc, char *argv[]) {
    int sock;
    char message[BUF_SIZE];
    socklen_t adr_sz;

    struct sockaddr_in serv_adr, from_adr;

    if (argc != 3) {
        printf("Usage: %s <IP> <message>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        error_handling("UDP socket creation error!\n");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(PORT);

    sendto(sock, argv[2], strlen(argv[2]), 0, (struct sockaddr *) &serv_adr, sizeof(serv_adr));

    adr_sz = sizeof(from_adr);
    ssize_t str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr *) &from_adr, &adr_sz);
    if (str_len < 0)
        error_handling("recvfrom() error!\n");
    printf("Sent to server: %s\n", message);

    close(sock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

