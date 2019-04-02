#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LINE 80 

int main(void)
{
    char *args[MAX_LINE/2 + 1];
    int should_run = 1;
    pid_t pid;

    while(should_run){
        printf("osh>");
        fflush(stdout);
        pid = fork();

        if (pid == 0){  // Child Process
            wait(NULL);
            printf("Child Finished");
        }
        else if (pid > 0)  // Parent process
        {
            execvp(args[0], args);
        }
        else{  // Failed 
            printf("Failed to fork");
        }
    }
    return 0;
}