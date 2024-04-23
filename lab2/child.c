#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Name: %s, PID: %d, PPID: %d\n", argv[0], getpid(), getppid());
    for (int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }
    sleep(10); 
    return 0;
}
