// common.c
#include "common.h"

CommandType parse_command(const char *cmd) {
    if (strncmp(cmd, "findfile", 8) == 0) {
        return FIND_FILE;
    } else if (strncmp(cmd, "sgetfiles", 9) == 0) {
        return SGET_FILES;
    } else if (strncmp(cmd, "dgetfiles", 9) == 0) {
        return DGET_FILES;
    } else if (strncmp(cmd, "getfiles", 8) == 0) {
        return GET_FILES;
    } else if (strncmp(cmd, "gettargz", 8) == 0) {
        return GET_TARGZ;
    } else if (strncmp(cmd, "quit", 4) == 0) {
        return QUIT;
    } else {
        return INVALID;
    }
}

bool find_file_recursive(const char *filename, const char *dir_path, char *filepath, size_t filepath_size) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return false;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char new_dir_path[BUFFER_SIZE];
                snprintf(new_dir_path, sizeof(new_dir_path), "%s/%s", dir_path, entry->d_name);
                if (find_file_recursive(filename, new_dir_path, filepath, filepath_size)) {
                    closedir(dir);
                    return true;
                }
            }
        } else if (entry->d_type == DT_REG) {
            if (strcmp(entry->d_name, filename) == 0) {
                snprintf(filepath, filepath_size, "%s/%s", dir_path, entry->d_name);
                closedir(dir);
                return true;
            }
        }
    }

    closedir(dir);
    return false;
}

bool find_file(const char *filename, char *filepath, size_t filepath_size) {
    // Get user's home directory
    const char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("Error getting user's home directory");
        return false;
    }

    // Search for the file in the directory tree rooted at the home directory
    return find_file_recursive(filename, home_dir, filepath, filepath_size);
}

int send_file_info(int sockfd, const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) < 0) {
        send_error(sockfd, "Error retrieving file info");
        return -1;
    }

    time_t creation_time = st.st_ctime;
    off_t file_size = st.st_size;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %ld %ld", filepath, file_size, creation_time);

    if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending file info");
        return -1;
    }
    return 0;
}

int send_tar_gz(int sockfd, const char *tar_gz_path) {
    int file_fd;
    off_t file_size;
    ssize_t bytes_sent;
    struct stat st;

    file_fd = open(tar_gz_path, O_RDONLY);
    if (file_fd < 0) {
        send_error(sockfd, "Error opening tar.gz file");
        return -1;
    }

    if (fstat(file_fd, &st) < 0) {
        send_error(sockfd, "Error retrieving tar.gz file info");
        close(file_fd);
        return -1;
    }

    file_size = st.st_size;
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %ld", tar_gz_path, file_size);

    if (send(sockfd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending tar.gz file info");
        close(file_fd);
        return -1;
    }

    off_t len = file_size;
    bytes_sent = sendfile(file_fd, sockfd, 0, &len, NULL, 0);
    if (bytes_sent < 0) {
        perror("Error sending tar.gz file");
        close(file_fd);
        return -1;
    }

    close(file_fd);
    return 0;
}

void send_error(int sockfd, const char *error_msg) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "Error: %s", error_msg);
    send(sockfd, buffer, strlen(buffer), 0);
}
