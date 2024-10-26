#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define SQLITE_STRING "SQLite format 3"

void check_func(const char* filepath) {
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    //determine file size
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET); 

    //allocate memory for file contents
    char *buffer = (char*)malloc(filesize + 1);
    if (buffer == NULL) {
        perror("malloc");
        fclose(file);
        return;
    }

    //read the entire file into the buffer
    size_t read_bytes = fread(buffer, 1, filesize, file);
    buffer[read_bytes] = '\0'; //null-terminate the string
    fclose(file);

    //strstr to find the string in the file
    if (strstr(buffer, SQLITE_STRING) != NULL) {
        printf("%s\n", filepath);
    }

    free(buffer);
}

void dir_traversal(const char* directory) {
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
