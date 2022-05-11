#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAP_SIZE 4096

// files for mapping
char* file1_name = "file1_mapped";
char* file2_name = "file2_mapped";

// empty string as a signal
char empty = 1;
char* empty_string = &empty;

// scan a string with unknown length
char* get_string() {
    int len = 0, capacity = 10;
    char* s = (char*)malloc(10 * sizeof(char));
    if (s == NULL) {
        perror("Can't read a string");
        exit(6);
    }

    char c;
    while ((c = getchar()) != '\n') {
        s[len++] = c;
        if (c == EOF) {
            break;
        }
        if (len == capacity) {
            capacity *= 2;
            s = (char*)realloc(s, capacity * sizeof(char));
            if (s == NULL) {
                perror("Can't read a string");
                exit(6);
            }
        }
    };
    s[len] = '\0';
    return s;
}


int main() {
    srand(time(NULL));

    // creating files for output of child processes
    printf("Enter file's name for child process 1: ");
    char* output_file1_name = get_string();

    printf("Enter file's name for child process 2: ");
    char* output_file2_name = get_string();

    int output_file1 = open(output_file1_name, O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE | S_IREAD);
    int output_file2 = open(output_file2_name, O_WRONLY | O_CREAT | O_TRUNC, S_IWRITE | S_IREAD);
    if (output_file1 < 0 || output_file2 < 0) {
        perror("Can't open file");
        exit(1);
    }


    // creating files for mapping
    int fd1 = open(file1_name, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    int fd2 = open(file2_name, O_RDWR | O_CREAT, S_IWRITE | S_IREAD);
    if (fd1 < 0 || fd2 < 0) {
        perror("Can't open file");
        exit(1);
    }

    // empty files can't be mapped, so we'll put our empty_string there
    if (write(fd1, empty_string, sizeof(empty_string)) < 0) {
        perror("Can't write to file");
        exit(1);
    }
    if (write(fd2, empty_string, sizeof(empty_string)) < 0) {
        perror("Can't write to file");
        exit(1);
    }

    // mapping files
    char* file1 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    char* file2 = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if (file1 == MAP_FAILED || file2 == MAP_FAILED) {
        perror("Can't map a file");
        exit(2);
    }


    // creating child processes
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("Can't create child process");
        exit(3);
    }

    if (pid1 > 0) { // parent
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("Can't create child process");
            exit(3);
        }

        if (pid2 > 0) { // parent

            while (1) {
                char* s = get_string();

                if (rand() % 100 + 1 <= 80) {
                    strcpy(file1, s);
                    if (s[0] == EOF) {
                        strcpy(file2, s);
                        break;
                    }
                }
                else {
                    strcpy(file2, s);
                    if (s[0] == EOF) {
                        strcpy(file1, s);
                        break;
                    }
                }
            }
            if (munmap(file1, MAP_SIZE) < 0 || munmap(file2, MAP_SIZE) < 0) {
                perror("Can't unmap files");
                exit(4);
            }
            if (close(fd1) < 0 || close(fd2) < 0) {
                perror("Can't close files");
                exit(5);
            }
            if (remove(file1_name) < 0 || remove(file2_name) < 0) {
                perror("Can't delete files");
                exit(6);
            }
        }
        else { // child2
            // redirecting output
            if (dup2(output_file2, STDOUT_FILENO) < 0) {
                perror("Can't redirect stdout for child process");
                exit(7);
            }

            char* arr [] = {"2", NULL};
            execv("child", arr);

            // it won't go here if child executes
            perror("Can't execute child process");
            exit(8);
        }
    }
    else { // child1
        // redirecting output
        if (dup2(output_file1, STDOUT_FILENO) < 0) {
            perror("Can't redirect stdout for child process");
            exit(7);
        }

        char* arr [] = {"1", NULL};
        execv("child", arr);

        // it won't go here if child executes
        perror("Can't execute child process");
        exit(8);
    }
}
