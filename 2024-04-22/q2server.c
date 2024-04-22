#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 50000
#define BUF_SIZE 256

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    time_t rawtime;
    struct tm *timeinfo;
    socklen_t addr_len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running. Waiting for client requests...\n");

    while (1) {
        addr_len = sizeof(client_addr);
        int bytes_received = recvfrom(sockfd, (char *) buffer, BUF_SIZE, 0, (struct sockaddr *) &client_addr,
                                      &addr_len);
        if (bytes_received < 0) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_received] = '\0';
        printf("Request received from client: %s\n", inet_ntoa(client_addr.sin_addr));

        rawtime = time(NULL);
        timeinfo = localtime(&rawtime);
        strftime(buffer, BUF_SIZE, "%Y-%m-%d %H:%M:%S", timeinfo);

        if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        printf("Response sent to client.\n");
    }

    close(sockfd);
    return 0;
}
