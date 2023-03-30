// mirror.c
#include "common.h"

void process_client(int sockfd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t recv_size = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (recv_size <= 0) {
            if (recv_size < 0) {
                perror("Error receiving command");
            }
            return;
        }

        buffer[recv_size] = '\0';
        CommandType cmd = parse_command(buffer);

        switch (cmd) {
            case FIND_FILE:
                // Implement FIND_FILE logic
                break;
            case SGET_FILES:
                // Implement SGET_FILES logic
                break;
            case DGET_FILES:
                // Implement DGET_FILES logic
                break;
            case GET_FILES:
                // Implement GET_FILES logic
                break;
            case GET_TARGZ:
                // Implement GET_TARGZ logic
                break;
            case QUIT:
                printf("Client has closed the connection\n");
                return;
            default:
                printf("Invalid command received: %s\n", buffer);
                send_error(sockfd, "Invalid command");
        }
    }
}

int main() {
    int sockfd, new_sockfd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, BACKLOG) < 0) {
        perror("Error listening on socket");
        exit(EXIT_FAILURE);
    }

    printf("Mirror is running, waiting for connections...\n");

    while (1) {
        client_addr_len = sizeof(client_addr);
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_sockfd < 0) {
            perror("Error accepting connection");
            continue;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        process_client(new_sockfd);
        close(new_sockfd);
    }

    close(sockfd);
    return 0;
}
