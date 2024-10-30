#include <stdio.h>      //for standard i/o functions
#include <stdlib.h>     //for memory allocation
#include <string.h>     //for string handling
#include <dirent.h>     //for directory entry functions
#include <sys/stat.h>   //for file status functions
#include <unistd.h>     //for posix api

#define SQLITE_STRING "SQLite format 3"  //string to search for
#define PATH_MAX 4096  //maximum path length

void check_func(const char* filepath) {
    FILE *file = fopen(filepath, "rb"); //open the file
    if (file == NULL) {
        perror("fopen"); //error opening file
        return;
    }

    fseek(file, 0, SEEK_END); //move pointer to end
    long filesize = ftell(file); //get file size
    fseek(file, 0, SEEK_SET); //reset pointer

    char *buffer = (char*)malloc(filesize + 1); //allocate memory
    if (buffer == NULL) {
        perror("malloc"); //error allocating memory
        fclose(file); //close file
        return;
    }

    size_t read_bytes = fread(buffer, 1, filesize, file); //read file
    buffer[read_bytes] = '\0'; //null-terminate
    fclose(file); //close file

    if (strstr(buffer, SQLITE_STRING) != NULL) {
        printf("found in: %s\n", filepath); //print path if found
    }

    free(buffer); //free memory
}

void dir_traversal(const char* directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory); //open directory

    if (dp == NULL) {
        perror("opendir"); //error opening directory
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[PATH_MAX]; //full path buffer
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        struct stat path_stat;
        if (lstat(fullpath, &path_stat) == -1) {
            perror("lstat"); //error getting status
            continue;
        }

        if (S_ISDIR(path_stat.st_mode)) {
            dir_traversal(fullpath); //traverse directories
        } else if (S_ISREG(path_stat.st_mode)) {
            check_func(fullpath); //check regular files
        }
    }

    closedir(dp); //close directory
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <directory>\n", argv[0]); //usage message
        return EXIT_FAILURE; //exit failure
    }

    dir_traversal(argv[1]); //start traversal

    return EXIT_SUCCESS; //exit success
}
