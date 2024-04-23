#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

extern char **environ;

void print_sorted_env() {
    int n = 0;
    for (char **env = environ; *env != 0; env++) {
        n++;
    }

    char **env_copy = malloc((n + 1) * sizeof(char *));
    for (int i = 0; i < n; i++) {
        env_copy[i] = environ[i];
    }
    env_copy[n] = NULL;

    qsort(env_copy, n, sizeof(char *), (__compar_fn_t)strcmp);

    for (int i = 0; i < n; i++) {
        printf("%s\n", env_copy[i]);
    }

    free(env_copy);
}

int main(int argc, char *argv[], char *envp[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <env file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    print_sorted_env();

    int child_counter = 0;
    while (1) {
        printf("Enter '+', '*', '&', or 'q' to quit: ");
        fflush(stdout); 

        char ch = getchar();
        if (ch == 'q') break;


        if (ch != '\n') {
            while (getchar() != '\n');
        }

        pid_t pid = fork();
        if (pid == 0) { 
            char child_name[20];
            sprintf(child_name, "child_%02d", child_counter);
            char *child_argv[] = {child_name, NULL};
            char *child_env[10];

            FILE *file = fopen(argv[1], "r");
            if (!file) {
                perror("Failed to open environment file");
                exit(EXIT_FAILURE);
            }

            char var[256];
            int i = 0;
            while (fgets(var, sizeof(var), file)) {
                var[strcspn(var, "\n")] = 0;
                    if (!(child_env[i] = getenv(var))) {
                    fprintf(stderr, "Variable %s not found in environment\n", var);
                } else {
                    i++;
                }
            }
            fclose(file);
            child_env[i] = NULL;

            char *child_path = NULL;
            if (ch == '+') {
                child_path = getenv("CHILD_PATH");
            }
            else if (ch == '*') {
                child_path = getenv("CHILD_PATH");
                for (int i = 0; envp[i] != NULL; i++) {
                    if (strncmp(envp[i], "CHILD_PATH=", 11) == 0) {
                        child_path = envp[i] + 11;
                        break;
                    }
                }
            } else if (ch == '&') {
                for (int i = 0; environ[i] != NULL; i++) {
                    if (strncmp(environ[i], "CHILD_PATH=", 11) == 0) {
                        child_path = environ[i] + 11;
                        break;
                    }
                }
            }

            if (!child_path) {
                fprintf(stderr, "CHILD_PATH is not set\n");
                exit(EXIT_FAILURE);
            }

            char child_exec_path[512];
            sprintf(child_exec_path, "%s/child", child_path);
            if (execve(child_exec_path, child_argv, child_env) == -1) {
                perror("execve failed");
                exit(EXIT_FAILURE);
            }
        } else if (pid > 0) {
            child_counter++;
            wait(NULL);
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
