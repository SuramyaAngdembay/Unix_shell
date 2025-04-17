#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* Max command length */

int main(void) {
    char *args[MAX_LINE / 2 + 1]; /* Command arguments (max 40 tokens + NULL) */
    char input[MAX_LINE];         /* Buffer for input */
    int should_run = 1;           /* Shell loop control */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        /* Read user input */
        if (!fgets(input, MAX_LINE, stdin)) {
            perror("fgets failed");
            continue;
        }

        /* Remove trailing newline */
        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        /* Check for "exit" command */
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        /* Parse input into arguments */
        int i = 0;
        char *token = strtok(input, " ");
        int background = 0;

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;
            } else {
                args[i++] = token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL; /* Null-terminate the argument array */

        /* Fork a child process */
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            /* Child process */
            if (execvp(args[0], args) == -1) {
                perror("Execution failed");
            }
            exit(1); /* Only if exec fails */
        } else {
            /* Parent process */
            if (!background) {
                wait(NULL); /* Wait for child to complete */
            }
        }
    }

    return 0;
}

