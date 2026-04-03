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
        printf("You typed: %s", line);
    }
}