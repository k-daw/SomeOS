#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 80 
#define MAX_HISTORY 10
#define INPUT_OPERATOR 0
#define OUTPUT_OPERATOR 1
#define PIPELINE_OPERATOR 2


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


int get_args(char *args[MAX_LINE/2 + 1], char * history[MAX_HISTORY], int * operator_index, int * operator_type)
{
    
    char *buff[41];
    int i=0;
    int n;
    
    fgets(buff, MAX_LINE, stdin);

    if (buff[0] == '\0')
        return 0;

    n = split(buff, args);

    if ((strncmp(args[0], "!!", 1)==0)){
        printf("%s\n", history);
        n = split(history, args);
        return n;
    }

    for (i=0; i < n; i++) printf("%s\t", args[i]);
    printf("\n");

    
    for (i = 0; i < n; i++){
        // printf("I AM HERE 4");
        if (strncmp(args[i], "<", 1)==0) {*(operator_type) = 0; break;}
        if (strncmp(args[i], ">", 1)==0) {*(operator_type) = 1; break;}
        if (strncmp(args[i], "|", 1)==0) {*(operator_type) = 2; break;}
    }
    if (i != n) *operator_index = i;

    * history =  * buff;
    return n;
}


int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char *buff[41];
    char * second_operands[40];
    int operator_index, operator_type;
    int i = 0, sz;
    int should_run = 1;
    int wait_for_child = 0;
    char *history[MAX_HISTORY];
    char * pch;
    pid_t pid;
    char *c = (char *) calloc(100, sizeof(char)); 
    
    
    // int fd = open("unsorted.txt",  O_RDONLY | O_CREAT);
    // sz = read(fd, c, 10); 
    // printf("called read(% d, c, 10).  returned that"
    //      " %s bytes  were read.\n", fd, c); 
    while(should_run){
        operator_index = -1;
        operator_type = -1;
        printf("osh>");
        
        fflush(stdout);
        int n = get_args(args, history, &operator_index, &operator_type);
        if (n > 0)
        {   
            wait_for_child = (strncmp(args[n-1], "&", 1)==0);
            if (wait_for_child) args[n-1] = NULL;
        } 
        else continue;
        pid = fork();

        if (pid == 0){  // Child Process
            int index = 0;
            printf("I AM HERE 4\n");
            if (operator_type >= 0){
                
                printf("%d/%d\n", operator_index, n);
                //for (i=0; i < n; i++) printf("%s\t", args[i]);
                args[operator_index] = NULL;
                for (i = operator_index+1; i < n; i++){
                    printf("%d/%d\n", i, n);
                    //printf("agrs[%d] = %s",i, args[i]);
                    second_operands[index++] = args[i];
                    args[i] = NULL;
                    printf("Operators %s", second_operands[index-1]);
                    printf("Again --> %d/%d\n", i, n);
                    }
            }
            printf("I AM HERE %d\n", operator_type);
            for (i=0; i < operator_index; i++) printf("%s\t", args[i]);
            if (operator_type == 0) 
            {
                int fd = open( second_operands[index-1],  O_RDONLY | O_CREAT);
                dup2(fd, STDIN_FILENO);
                close(fd);
                if(execvp(args[0], args) == -1){
                    printf("Command is not avaliable\n");
                }
                continue;
            }
            
            if (operator_type == 1) continue;
            if (operator_type == 2) continue;
            
            
            if(execvp(args[0], args) == -1){
                printf("Command is not avaliable\n");
            }
            exit(1);   
        }
        else if (pid > 0)  // Parent process
        {   
            if(!wait_for_child) wait(NULL);
            else continue;
        }
        else{  // Failed 
            printf("Failed to fork");
        }
    }
    return 0;
}