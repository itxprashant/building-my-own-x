#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

        printf("Args[0] is %s\n", args[0]);
    }

}