// server.c
#include "common.h"

// Utility functions (add these to the common.h file)
int send_file_info(int sockfd, const char *filepath);
int send_tar_gz(int sockfd, const char *command);
void send_error(int sockfd, const char *error_msg);

void processclient(int client_fd) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t recv_len = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (recv_len <= 0) {
            close(client_fd);
            return;
        }

        CommandType cmd_type = parse_command(buffer);

        switch (cmd_type) {
            case FIND_FILE: {
                char *filename = buffer + strlen("findfile ") + 1;
                char filepath[BUFFER_SIZE];
                if (find_file(filename, filepath, sizeof(filepath))) {
                    send_file_info(client_fd, filepath);
                } else {
                    send_error(client_fd, "File not found");
                }
                break;
            }
            case SGET_FILES: {
                // Implement sgetfiles command handling
                break;
            }
            case DGET_FILES: {
                // Implement dgetfiles command handling
                break;
            }
            case GET_FILES: {
                // Implement getfiles command handling
                break;
            }
            case GET_TARGZ: {
                // Implement gettargz command handling
                break;
            }
            case QUIT: {
                close(client_fd);
                return;
            }
            case INVALID:
            default: {
                send_error(client_fd, "Invalid command");
                break;
            }
        }
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Cannot bind socket");
        close(sockfd);
        return 1;
    }

    if (listen(sockfd, MAX_CLIENTS) < 0) {
        perror("Cannot listen on socket");
        close(sockfd);
        return 1;
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            perror("Cannot accept client connection");
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("Cannot fork child process");
            close(client_fd);
        } else if (pid == 0) {
            close(sockfd);
            processclient(client_fd);
            close(client_fd);
            exit(0);
        } else {
            close(client_fd);
        }
    }

    close(sockfd);
    return 0;
}
