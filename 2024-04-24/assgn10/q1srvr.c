//Q1 server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 50000
#define MAX_STRING_LENGTH 100

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_STRING_LENGTH] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket Error!\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt error!\n");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed!\n");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen error!\n");
        exit(EXIT_FAILURE);
    }

    printf("\nServer listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept error!\n");
            exit(EXIT_FAILURE);
        }
        printf("\nClient connected!\n");

        while (1) {
            valread = read(new_socket, buffer, MAX_STRING_LENGTH);
            if (valread <= 0) {
                break;
            }

            printf("\tReceived: %s\n", buffer);
            reverse_string(buffer);

            send(new_socket, buffer, strlen(buffer), 0);
            printf("\tReversed: %s\n", buffer);
            memset(buffer, 0, strlen(buffer));
        }

        printf("\nClient disconnected!\n");
        close(new_socket);
    }

    return 0;
}
