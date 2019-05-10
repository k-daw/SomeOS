#include<semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_WAITING_STUDENTS 3
#define MAX_STUDENTS 1

struct node{
    int student_id;
    struct node * next;
} ;

struct node* head = NULL;
struct node* tail = NULL;


void *simulate_student(int student_id);
void *simulate_ta(void * param);
void *sit_with_ta(int time, int student_id);
void wake_ta_up();
int *ask_for_ta(int student_id);
void *go_to_ta(void * param);
void *wait_for_ta(int student_id);
int check_for_students(void * param);
void simulate_university(void * param);
void programming();


pthread_t STUDENTS[MAX_STUDENTS];
pthread_t TA;
pthread_attr_t attr;
pthread_cond_t cond_var;


pthread_mutex_t mutex;
sem_t *sem;



int main(){
    head = (struct node*)malloc(sizeof(struct node));
    tail = head;
    int *noob;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_var, NULL);
    sem = sem_open("SEM", 0, 0, 4);

    simulate_university(NULL);

    for (int i = 0; i < MAX_STUDENTS; i++)
        pthread_join(STUDENTS[i], NULL);
    printf("Finished Simulation\n");
    return 0;
}

void simulate_university(void * param){
    printf("Simulating University\n");
    int i;
    pthread_attr_init(&attr);
    pthread_create(&TA, &attr, simulate_ta, NULL);  // Create TA Thread

    for( i=0; i< MAX_STUDENTS; i++)
    {
        
        pthread_create(&STUDENTS[i], &attr, simulate_student, i);
    }  // Create STUDENT Thread
}

void *simulate_student(int student_id){
    //while(1){
        int chance = 3;//rand()%10;
        
        printf("Creating Thread for student: %d\n", student_id);
        if (chance < 4)
        { 
            printf("Asking for TA: %d\n", student_id);
            ask_for_ta(student_id);
        }
        else programming();
    //}
    pthread_exit(0);
}

void *simulate_ta(void * param){
    printf("Created Thread for TA\n");
    while(1){
        int time = rand() % 3;
        pthread_mutex_lock(&mutex);
        while(sem==3) pthread_cond_wait(&cond_var, &mutex);
        pthread_mutex_unlock(&mutex);
        check_for_students(NULL);
    }
    pthread_exit(0);
}

void *sit_with_ta(int time, int student_id){
    
    pthread_mutex_lock(&mutex);
    wait(time);
    printf("TA is sitting with Student: %d", student_id);
    pthread_mutex_unlock(&mutex);
}

int *ask_for_ta(int student_id){
    if(sem_trywait(sem) == -1) return 0;
    else{
        printf("Waiting for TA: %d\n", student_id);
        wait_for_ta(student_id);
        return 1;
    }
}

void programming(){
    sleep(5);
}

int check_for_students(void * param)
{
    struct node* temp;
    if(head != NULL)
    {
        temp = head;
        sit_with_ta(5, head->student_id);  
        head = head->next;
        free(temp);  // Free memory 
        return  1;
    }
    return 0;

}
void *wait_for_ta(int student_id){
    struct node* new_student;
    new_student = (struct node*)malloc(sizeof(struct node));
    new_student -> student_id = student_id;
    new_student -> next = NULL;
    tail = new_student;
}
void *go_to_ta(void * param){

    return NULL;
}