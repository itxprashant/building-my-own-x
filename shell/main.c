#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 1024

typedef enum {
    TOK_WORD,
    TOK_PIPE,
    TOK_REDIRECT_IN,
    TOK_REDIRECT_OUT,
    TOK_REDIRECT_APPEND,
    TOK_BG,
    TOK_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;


int tokenize(const char *line, Token *out, int max_tokens, char *word_buffer, size_t word_cap){
    int token_count = 0;
    size_t word_len = 0;
    char *word_start = NULL;

    for (int i = 0; line[i] != '\0'; i++) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
            if (word_len > 0) {
                out[token_count].type = TOK_WORD;
                out[token_count].value = word_buffer;
                token_count++;
                word_len = 0;
            }
        } else {
            if (word_len == 0) {
                word_start = &line[i];
            }
            word_len++;
            if (word_len >= word_cap) {
                word_cap *= 2;
                word_buffer = realloc(word_buffer, word_cap);
            }
        }
    }
    if (word_len > 0) {
        out[token_count].type = TOK_WORD;
        out[token_count].value = word_buffer;
        token_count++;
    }
    return token_count;
}



int main() {

  setenv("SHELL", "myshell", 1);

  char line[MAX_LINE];

  while (true) {
    Token tokens[MAX_LINE];
    int token_count = 0;
    
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

    if (args[0] == NULL)
      continue;

    for (int j = 0; args[j] != NULL; j++) {
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
      } else if (strcmp(args[1], "-") == 0) {
        char *old_dir = getenv("OLDPWD");
        if (old_dir != NULL) {
          target_dir = old_dir;
        } else {
          printf("cd: OLDPWD not set\n");
          continue;
        }
      } else {
        target_dir = args[1];
      }

      if (chdir(target_dir) == -1) {
        perror("cd");
      }

      continue;
    }

    if (strcmp(args[0], "pwd") == 0) {
      char *current_dir = getcwd(NULL, 0);
      printf("%s\n", current_dir);
      free(current_dir);
      continue;
    }

    if (strcmp(args[0], "export") == 0) {
      if (args[1] == NULL) {
        printf("export: missing operand\n");
        continue;
      }

      if (setenv(args[1], args[2], 1) == -1) {
        perror("setenv");
      }
      continue;
    }

    pid_t pid = fork();

    if (pid == 0) {
      if (execvp(args[0], args) == -1) {
        perror("execvp");
      }
      exit(EXIT_FAILURE);
    } else if (pid < 0) {
      perror("Fork failed");
    } else {
      wait(NULL);
    }
  }
  return EXIT_SUCCESS;
}