#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80 

int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char * line;
    int i = 0;
    int should_run = 1;
    pid_t pid;
    char * pch;

    while(should_run){
        printf("osh>");
        pch = strtok (line," ");
        while (pch != NULL)
        {
            printf ("%s\n",pch);
            args[i++] = pch;
            printf ("%s\n",args[i-1]);
            pch = strtok (NULL, " ");
        }
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