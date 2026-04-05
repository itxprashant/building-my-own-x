#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024

int main() {

    char line[MAX_LINE];

    while (true) {

        printf("myshell> ");

        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            return EXIT_FAILURE;
        }

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0) {
            return EXIT_SUCCESS;
        }

        // tokenization
        char *args[64];
        int arg_count = 0;

        args[arg_count] = strtok(line, " ");
        while (args[arg_count] != NULL && arg_count < 63) {
            arg_count++;
            args[arg_count] = strtok(NULL, " ");
        }

        if (args[0] == NULL) continue;

        pid_t pid = fork();

        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                perror("execvp");
            }
            exit(EXIT_FAILURE);
        } else if (pid < 0){
            perror("Fork failed");
        } else {
            wait(NULL);
        }
    }
    return EXIT_SUCCESS;
}