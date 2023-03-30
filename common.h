// common.h
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_PORT 8080
#define MIRROR_PORT 8081
#define BUFFER_SIZE 1024
#define BACKLOG 10
#define MAX_CLIENTS 4
#define MAX_EXT 6
#define TMP_DIR "/tmp"

typedef enum {
    FIND_FILE,
    SGET_FILES,
    DGET_FILES,
    GET_FILES,
    GET_TARGZ,
    QUIT,
    INVALID
} CommandType;

// Function declarations
CommandType parse_command(const char *cmd);
bool find_file(const char *filename, char *filepath, size_t filepath_size);
int send_file_info(int sockfd, const char *filepath);
int send_tar_gz(int sockfd, const char *command);
void send_error(int sockfd, const char *error_msg);

#endif
