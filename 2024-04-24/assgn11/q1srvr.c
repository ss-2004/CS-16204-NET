// Q1 Server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 50000
#define MAX_EXPRESSION_LENGTH 100

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed error!\n");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt error!\n");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed error!\n");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen error!\n");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept error!\n");
            exit(EXIT_FAILURE);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork error!\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            close(server_fd);

            char buffer[MAX_EXPRESSION_LENGTH] = {0};
            while (1) {
                int valread = read(new_socket, buffer, MAX_EXPRESSION_LENGTH);
                if (valread <= 0) {
                    break;
                }

                printf("Received: %s\n", buffer);
                reverse_string(buffer);

                send(new_socket, buffer, strlen(buffer), 0);
                printf("Reversed: %s\n", buffer);
            }

            close(new_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(new_socket);
        }
    }

    return 0;
}
