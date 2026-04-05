#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024

int main() {

    setenv("SHELL", "myshell", 1); 

    char line[MAX_LINE];

    while (true) {
        char *current_dir = getcwd(NULL, 0);
        printf("myshell> %s > ", current_dir);

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

        for (int j=0; args[j] != NULL; j++) {
            if (args[j][0] == '$') {
                char *var_name = &args[j][1];
                char *var_value = getenv(var_name);

                if (var_value != NULL) {
                    args[j] = var_value;
                } else {
                    args[j] = "";
                }
            }
        }

        if (strcmp(args[0], "cd") == 0) {

            char *target_dir;
            if (args[1] == NULL) {
                target_dir = getenv("HOME");
            } else if (strcmp(args[1], "~") == 0) {
                target_dir = getenv("HOME");
            } else {
                target_dir = args[1];
            }
            
            if (chdir(target_dir) == -1) {
                perror("cd");
            }

            continue;
        }

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