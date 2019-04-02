#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 80 



int split(char *str, char *arr[41]){
    int beginIndex = 0;
    int endIndex;
    int maxWords = 10;
    int wordCnt = 0;

    while(1){
        while(isspace(str[beginIndex])){
            ++beginIndex;
        }
        if(str[beginIndex] == '\0')
            break;
        endIndex = beginIndex;
        while (str[endIndex] && !isspace(str[endIndex])){
            ++endIndex;
        }
        int len = endIndex - beginIndex;
        char *tmp = calloc(len + 1, sizeof(char));
        memcpy(tmp, &str[beginIndex], len);
        arr[wordCnt++] = tmp;
        beginIndex = endIndex;
        if (wordCnt == maxWords)
            break;
    }
    return wordCnt;
}


int get_args(char *args[MAX_LINE/2 + 1]){
    char *buff[41];
    int i;

    fgets(buff, MAX_LINE, stdin);
    int n = split(buff, args);
    for(i = 0; i < n; ++i){
        puts(args[i]);
        free(args[i]);
    }
    return n;
}


int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char *buff[41];
    int wait = 0;
    int i = 0;
    int should_run = 1;
    char * pch;
    pid_t pid;

    while(should_run){
        printf("osh>");
        int n = get_args(args);

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
        
        fflush(stdout);
       
    }
    return 0;
}