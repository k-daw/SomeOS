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
#define MAX_HISTORY 100
#define INPUT_OPERATOR 0
#define OUTPUT_OPERATOR 1
#define PIPELINE_OPERATOR 2
#define PIPE_BUFFER_SIZE 200
#define READ_END 0
#define WRITE_END 1

int PIPED_CHILD = 0;
char buff[100];
char HISTORY[100];

int split(char *str, char *arr[41]){
    
    int beginIndex = 0;
    int endIndex;
    int maxWords = 100;
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


int get_args(char *args[MAX_LINE/2 + 1], int * operator_index, int * operator_type)
{
    int i=0;
    int n;
    
    fgets(buff, MAX_LINE, stdin);

    if (buff[0] == '\0')
        return 0;

    n = split(buff, args);
    int from_history = (strncmp(args[0], "!!", 1)==0);
    if (from_history){
        printf("%s\n", HISTORY);
        n = split(HISTORY, args);
    }
    
    for (i = 0; i < n; i++){
        // printf("I AM HERE 4");
        if (strncmp(args[i], "<", 1)==0) {*(operator_type) = 0; break;}
        if (strncmp(args[i], ">", 1)==0) {*(operator_type) = 1; break;}
        if (strncmp(args[i], "|", 1)==0) {*(operator_type) = 2; break;}
    }
    if (i != n) *operator_index = i;

    if (!from_history) strcpy(HISTORY, buff);
    
    return n;
}


int main(void)
{
    char *args[MAX_LINE/2 + 1];
    char *buff[41];
    char * second_operands[40];
    int operator_index, operator_type;
    int i = 0;
    int should_run = 1;
    int wait_for_child = 0;
    pid_t pid, pid_pipe;
    
    char write_msg[PIPE_BUFFER_SIZE];
    char read_msg[PIPE_BUFFER_SIZE];
    int fd_pipe[2];
    int saved_stdout;
    saved_stdout = dup(1);
    //char *c = (char *) calloc(100, sizeof(char)); 
    
    

    while(should_run){
        operator_index = -1;
        operator_type = -1;
        printf("osh>");
        
        fflush(stdout);
        int n = get_args(args, &operator_index, &operator_type);
        if (n > 0)
        {   
            wait_for_child = (strncmp(args[n-1], "&", 1)==0);
            if (wait_for_child) args[n-1] = NULL;
        } 
        else continue; 
        
        pid = fork();
        if (pid == 0){  // Child Process
            int index = 0;
            if (operator_type >= 0){
                args[operator_index] = NULL;
                for (i = operator_index+1; i < n; i++){
                    second_operands[index++] = args[i];
                    args[i] = NULL;
                }
            }

            if (operator_type == 0) {
                int fd = open( second_operands[index-1],  O_RDONLY);
                if (fd == -1) {
                    printf("Can't find file specified\n");
                    continue;
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
                if(execvp(args[0], args) == -1){
                    printf("Command is not avaliable\n");
                }
                continue;
            }
            
            if (operator_type == 1)
            {
                int fd = open(second_operands[index-1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                if(execvp(args[0], args) == -1){
                    printf("Command is not avaliable\n");
                }
                continue;
            }
            if (operator_type == 2){
                printf("Pipe\n");
                if (pipe(fd_pipe) == -1) {
                    fprintf(stderr,"Pipe failed");
                    continue;
                }
   
                for(i = 0; i < operator_index; i++) printf("args[%d] : %s\n", i, second_operands[i]);
                
                pid_pipe = fork();
                if (pid_pipe < 0) fprintf(stderr, "Fork Failed");
                if (pid_pipe == 0){
                    close(fd_pipe[WRITE_END]);
                    dup2(fd_pipe[READ_END], STDIN_FILENO);
                    dup2(saved_stdout, 1);
                    printf("Second Operads %s", second_operands[0]);
                    close(saved_stdout);
                    if(execvp(second_operands[0], second_operands) == -1){
                            printf("Piped Command is not failed to execute\n");
                            continue;
                        }
                        close(fd_pipe[READ_END]);
                    exit(1); 
                }
                close(fd_pipe[READ_END]);
                dup2(fd_pipe[WRITE_END], STDOUT_FILENO);
                close(fd_pipe[WRITE_END]);
                if(execvp(args[0], args) == -1){
                    printf("Command is not avaliable\n");
                }
                
                continue;
            }
            
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