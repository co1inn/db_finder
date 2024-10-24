#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define SQLITE_HEADER "SQLite format 3"
#define HEADER_SIZE 16

void check_func(const char* filepath){
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    char header[HEADER_SIZE + 1] = {0};
    size_t read_bytes = fread(header, 1, HEADER_SIZE, file);
    fclose(file);

    if (read_bytes == HEADER_SIZE && strncmp(header, SQLITE_HEADER, HEADER_SIZE) == 0) {
        printf("%s\n", filepath);
    }
}

void dir_traversal(const char* directory){
        struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        struct stat path_stat;
        if (stat(fullpath, &path_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(path_stat.st_mode)) {
            dir_traversal(fullpath);
        } else if (S_ISREG(path_stat.st_mode)) {
            check_func(fullpath);
        }
    }

    closedir(dp);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    dir_traversal(argv[1]);

    return EXIT_SUCCESS;
}