#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>  /* for open() */

#define MAX_LINE 80  /* Maximum length of command */

/* Main function implementing the shell loop */
int main(void) {
    char *args[MAX_LINE / 2 + 1];      /* Command arguments */
    char input[MAX_LINE];              /* Input buffer */
    char last_command[MAX_LINE] = "";  /* History buffer */
    int should_run = 1;                /* Loop control flag */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        if (!fgets(input, MAX_LINE, stdin)) {
            perror("fgets failed");
            continue;
        }

        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        /* Handle history recall using "!!" */
        if (strcmp(input, "!!") == 0) {
            if (strlen(last_command) == 0) {
                printf("No commands in history.\n");
                continue;
            } else {
                printf("%s\n", last_command);
                strcpy(input, last_command);
            }
        } else {
            strcpy(last_command, input);
        }

        /* Exit shell */
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        /* Check for pipe operator */
        int is_pipe = 0;
        char *cmd1 = NULL, *cmd2 = NULL;
        char *pipe_ptr = strchr(input, '|');
        if (pipe_ptr != NULL) {
            is_pipe = 1;
            *pipe_ptr = '\0';
            cmd1 = input;
            cmd2 = pipe_ptr + 1;

            while (*cmd2 == ' ') cmd2++;
        }

        /* Handle pipe command */
        if (is_pipe) {
            char *args1[MAX_LINE / 2 + 1];
            char *args2[MAX_LINE / 2 + 1];
            int i = 0;

            char *token = strtok(cmd1, " ");
            while (token != NULL) {
                args1[i++] = token;
                token = strtok(NULL, " ");
            }
            args1[i] = NULL;

            i = 0;
            token = strtok(cmd2, " ");
            while (token != NULL) {
                args2[i++] = token;
                token = strtok(NULL, " ");
            }
            args2[i] = NULL;

            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe failed");
                continue;
            }

            pid_t pid1 = fork();
            if (pid1 == 0) {
                /* First child writes to pipe */
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                execvp(args1[0], args1);
                perror("execvp 1 failed");
                exit(1);
            }

            pid_t pid2 = fork();
            if (pid2 == 0) {
                /* Second child reads from pipe */
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[1]);
                close(pipefd[0]);
                execvp(args2[0], args2);
                perror("execvp 2 failed");
                exit(1);
            }

            /* Parent process closes pipe and waits */
            close(pipefd[0]);
            close(pipefd[1]);
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
            continue;
        }

        /* Initialize redirection and background flags */
        int redirect_output = 0, redirect_input = 0, background = 0;
        char *filename = NULL;

        /* Check for redirection symbols */
        char *out = strchr(input, '>');
        char *in = strchr(input, '<');

        if (out != NULL) {
            redirect_output = 1;
            *out = '\0';
            filename = strtok(out + 1, " \t");
        } else if (in != NULL) {
            redirect_input = 1;
            *in = '\0';
            filename = strtok(in + 1, " \t");
        }

        /* Parse command into arguments */
        int i = 0;
        char *token = strtok(input, " ");
        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;
            } else {
                args[i++] = token;
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        /* Built-in command: cd */
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

        /* Fork a child process to run the command */
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            /* Handle output redirection */
            if (redirect_output && filename != NULL) {
                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd < 0) {
                    perror("open output");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            /* Handle input redirection */
            else if (redirect_input && filename != NULL) {
                int fd = open(filename, O_RDONLY);
                if (fd < 0) {
                    perror("open input");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            /* Execute the command */
            execvp(args[0], args);
            perror("Execution failed");
            exit(1);
        } else {
            /* Parent waits if not a background task */
            if (!background) {
                wait(NULL);
            }
        }
    }

    return 0;
}

