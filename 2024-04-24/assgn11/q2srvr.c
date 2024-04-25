// Q2 Server

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 50000
#define MAX_EXPRESSION_LENGTH 100

struct StackNode {
    int data;
    struct StackNode *next;
};

struct StackNode *newNode(int data) {
    struct StackNode *stackNode = (struct StackNode *) malloc(sizeof(struct StackNode));
    stackNode->data = data;
    stackNode->next = NULL;
    return stackNode;
}

void push(struct StackNode **root, int data) {
    struct StackNode *stackNode = newNode(data);
    stackNode->next = *root;
    *root = stackNode;
}

int pop(struct StackNode **root) {
    if (*root == NULL) {
        fprintf(stderr, "Error: Stack underflow\n");
        exit(EXIT_FAILURE);
    }
    struct StackNode *temp = *root;
    *root = (*root)->next;
    int popped = temp->data;
    free(temp);
    return popped;
}

int peek(struct StackNode *root) {
    if (root == NULL) {
        fprintf(stderr, "Error: Stack empty\n");
        exit(EXIT_FAILURE);
    }
    return root->data;
}

int evaluate_expression(const char *expression) {
    struct StackNode *values = NULL;
    struct StackNode *ops = NULL;

    for (int i = 0; expression[i]; i++) {
        if (expression[i] == ' ')
            continue;

        if (isdigit(expression[i])) {
            int num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;

            push(&values, num);
        } else if (expression[i] == '(') {
            push(&ops, expression[i]);
        } else if (expression[i] == ')') {
            while (peek(ops) != '(') {
                int val2 = pop(&values);
                int val1 = pop(&values);
                char op = pop(&ops);

                switch (op) {
                    case '+':
                        push(&values, val1 + val2);
                        break;
                    case '-':
                        push(&values, val1 - val2);
                        break;
                    case '*':
                        push(&values, val1 * val2);
                        break;
                    case '/':
                        push(&values, val1 / val2);
                        break;
                }
            }
            pop(&ops);
        } else {
            while (ops != NULL && peek(ops) != '(' && ((expression[i] == '*' || expression[i] == '/') ||
                                                       (peek(ops) == '+' || peek(ops) == '-'))) {
                int val2 = pop(&values);
                int val1 = pop(&values);
                char op = pop(&ops);

                switch (op) {
                    case '+':
                        push(&values, val1 + val2);
                        break;
                    case '-':
                        push(&values, val1 - val2);
                        break;
                    case '*':
                        push(&values, val1 * val2);
                        break;
                    case '/':
                        push(&values, val1 / val2);
                        break;
                }
            }

            push(&ops, expression[i]);
        }
    }

    while (ops != NULL) {
        int val2 = pop(&values);
        int val1 = pop(&values);
        char op = pop(&ops);

        switch (op) {
            case '+':
                push(&values, val1 + val2);
                break;
            case '-':
                push(&values, val1 - val2);
                break;
            case '*':
                push(&values, val1 * val2);
                break;
            case '/':
                push(&values, val1 / val2);
                break;
        }
    }

    return pop(&values);
}


void *handle_client(void *arg) {
    int client_socket = *((int *) arg);
    char expression[MAX_EXPRESSION_LENGTH];
    char result[MAX_EXPRESSION_LENGTH];

    while (1) {
        int bytes_received = recv(client_socket, expression, MAX_EXPRESSION_LENGTH, 0);
        if (bytes_received <= 0) {
            break;
        }

        expression[bytes_received] = '\0';
        double result_value = evaluate_expression(expression);
        sprintf(result, "%.2lf", result_value);
        send(client_socket, result, strlen(result), 0);
        memset(expression, 0, strlen(expression));
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Client connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void *) &new_socket) < 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }

        pthread_detach(thread);
    }

    return 0;
}
