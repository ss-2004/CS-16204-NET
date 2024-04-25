//Q1 Client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 50000
#define MAX_STRING_LENGTH 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <string>\n", argv[0]);
        return 1;
    }

    char *ip_address = argv[1];
    char *string = argv[2];
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[MAX_STRING_LENGTH] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock, string, strlen(string), 0);
    printf("Sent: %s\n", string);

    valread = read(sock, buffer, MAX_STRING_LENGTH);
    printf("Reversed: %s\n", buffer);
    return 0;
}
