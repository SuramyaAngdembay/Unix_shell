#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* Max command length */

int main(void) {
    char *args[MAX_LINE / 2 + 1];     /* Command arguments */
    char input[MAX_LINE];             /* Buffer for input */
    char last_command[MAX_LINE] = ""; /* History buffer */
    int should_run = 1;               /* Shell loop control */

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
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        /* Handle "!!" history feature */
        if (strcmp(input, "!!") == 0) {
            if (strlen(last_command) == 0) {
                printf("No commands in history.\n");
                continue;
            } else {
                printf("%s\n", last_command);  // Echo the previous command
                strcpy(input, last_command);   // Use it as input
            }
        } else {
            strcpy(last_command, input); // Save this command to history
        }

        /* Check for "exit" command */
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        /* Parse input into arguments */
        int i = 0;
        int background = 0;
        char *token = strtok(input, " ");

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;
            } else {
                args[i++] = token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL; /* Null-terminate the argument array */

        /* Handle built-in "cd" command */
        if (args[0] && strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "osh: expected argument to \"cd\"\n");
            } else {
                if (chdir(args[1]) != 0) {
                    perror("osh");
                }
            }
            continue;
        }

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

