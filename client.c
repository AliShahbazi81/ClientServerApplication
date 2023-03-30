// client.c
#include "common.h"

void print_usage() {
    printf("Usage:\n");
    printf("findfile <filename>\n");
    printf("sgetfiles <size1> <size2> <-u>\n");
    printf("dgetfiles <date1> <date2> <-u>\n");
    printf("getfiles <file1> <file2> ... <file6> <-u>\n");
    printf("gettargz <extension list (up to 6)> <-u>\n");
    printf("quit\n");
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter command: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            continue;
        }

        CommandType cmd = parse_command(buffer);
        if (cmd == INVALID) {
            printf("Invalid command.\n");
            print_usage();
            continue;
        }

        if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending command");
            break;
        }

        if (cmd == QUIT) {
            printf("Exiting...\n");
            break;
        }

        ssize_t recv_size = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (recv_size < 0) {
            perror("Error receiving response");
            break;
        }

        buffer[recv_size] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
