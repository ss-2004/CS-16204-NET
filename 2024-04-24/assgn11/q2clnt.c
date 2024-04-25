// Q2 client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#define MAX_EXPRESSION_LENGTH 100
#define SENTINEL_VALUE "exit"

char *resolve_hostname(const char *hostname) {
    struct hostent *host_entry;
    struct in_addr **addr_list;
    char *ip = NULL;

    if ((host_entry = gethostbyname(hostname)) == NULL) {
        perror("gethostbyname error!\n");
        return NULL;
    }

    addr_list = (struct in_addr **) host_entry->h_addr_list;
    if (addr_list[0] != NULL) {
        ip = strdup(inet_ntoa(*addr_list[0]));
    }

    return ip;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        return 1;
    }

    const char *hostname = argv[1];
    const char *port = argv[2];
    char expression[MAX_EXPRESSION_LENGTH];
    char answer[MAX_EXPRESSION_LENGTH];
    char *server_ip;

    server_ip = resolve_hostname(hostname);
    if (server_ip == NULL) {
        fprintf(stderr, "Error: Unable to resolve hostname!\n");
        return 1;
    }

    printf("Server IP address: %s\n", server_ip);
    printf("Server Port: %s\n", port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error!\n");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect error!\n");
        return 1;
    }

    while (1) {
        printf("Enter an arithmetic expression to calculate (or '%s' to exit): ", SENTINEL_VALUE);
        fgets(expression, MAX_EXPRESSION_LENGTH, stdin);
        expression[strcspn(expression, "\n")] = '\0';

        if (strcmp(expression, SENTINEL_VALUE) == 0) {
            break;
        }

        send(sock, expression, strlen(expression), 0);

        int bytes_received = recv(sock, answer, MAX_EXPRESSION_LENGTH, 0);
        if (bytes_received < 0) {
            perror("recv");
            break;
        }

        answer[bytes_received] = '\0';

        printf("Answer: %s\n", answer);
    }

    close(sock);
    free(server_ip);

    return 0;
}
