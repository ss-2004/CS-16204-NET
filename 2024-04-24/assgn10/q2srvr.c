// Q2 server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    const char *port = argv[1];
    char expression[MAX_EXPRESSION_LENGTH];
    char result[MAX_EXPRESSION_LENGTH];
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
    address.sin_port = htons(atoi(port));

    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed error!\n");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen error!\n");
        exit(EXIT_FAILURE);
    }

    printf("\n\tServer listening on port %s\n", port);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
            perror("accept error!\n");
            exit(EXIT_FAILURE);
        }

        printf("\tClient connected: %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        while (1) {
            int bytes_received = recv(new_socket, expression, MAX_EXPRESSION_LENGTH, 0);
            if (bytes_received <= 0) {
                break;
            }

            expression[bytes_received] = '\0';
            double result_value = evaluate_expression(expression);
            sprintf(result, "%.2lf", result_value);

            send(new_socket, result, strlen(result), 0);
            memset(expression, 0, strlen(expression));
        }

        printf("\tClient disconnected\n");
        close(new_socket);
    }

    return 0;
}
