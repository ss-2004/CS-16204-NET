#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    strcpy(buffer, "TimeRequest");
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    int bytes_received = recvfrom(sockfd, (char *) buffer, BUF_SIZE, 0, NULL, NULL);
    if (bytes_received < 0) {
        perror("Receive failed");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Time of day received from server: %s\n", buffer);
    close(sockfd);
    return 0;
}
