#include<semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 3
#define MAX_WAITING_STUDENTS 3
#define MAX_STUDENTS 10

void * simulate_student(void *);
void * simulate_ta(void *);
int go_to_ta();
void sit_with_student();
void programming(int);



pthread_t STUDENTS[MAX_STUDENTS];
pthread_t TA;
pthread_attr_t attr;

sem_t sem_student;  // for accessing the chair
sem_t sem_ta;  // for waking up the TA
sem_t student_waiting[3];
pthread_mutex_t mutex_waiting;  // for locking the  count

int student_ids[10];
// QUEUE OPERATIONS // 
int queue_chairs[MAX];
int rear = - 1;
int front = - 1;
int waiting_count = 0;

int insert();
void delete();
int main(int argc, char **argv)
{   
    printf("Entered Main \n");
    pthread_mutex_init(&mutex_waiting, NULL);
    printf("Creating Semaphores\n");
    sem_init(&sem_ta, 0, 0);  // Initially TA is sleeping
    sem_init(&sem_student, 0, 1); // Initially asking for a chair is possible
    int i;
    for (i=0 ; i<MAX_WAITING_STUDENTS; i++) sem_init(&student_waiting[i], 0,1);
    printf("Creating Threads\n");
    pthread_create(&TA, NULL, simulate_ta, NULL);  // Create TA Thread
    for( i=0; i< MAX_STUDENTS; i++)
    {
        student_ids[i] = i;
        pthread_create(&STUDENTS[i], NULL, simulate_student, (void *) &student_ids[i]);
    }  // Create STUDENT Thread

    pthread_join(TA, NULL);
    for (int i = 0; i < MAX_STUDENTS; i++)
    {
        
        pthread_join(STUDENTS[i], NULL);
    }
    sem_destroy(&sem_ta);
    sem_destroy(&sem_student);
    return 0;
}

void *simulate_student(void * param){
    int programing_time;
    int student_id = *((int *)param);
    printf("Created Thread for student: %d\n", student_id);

    while(1){
        programing_time = rand()%10;
        //printf("Student %d is programing for %d \n", student_id, programing_time);
        programming(programing_time);
        // printf("Student %d is going to TA\n", student_id);
        go_to_ta(student_id);
        
    }
}

 
void *simulate_ta(void *param){
    printf("Created Thread for TA\n");
    while(1){
        pthread_mutex_lock(&mutex_waiting);
        printf("TA IS CHECKING FOR STUDENTS: %d\n", waiting_count);
        if(waiting_count) sit_with_student();
        pthread_mutex_unlock(&mutex_waiting);
        printf("TA IS SLEEPING\n");
        sem_wait(&sem_ta); // Sleep or busy waiting until a student come
        printf("Student Woke Up TA\n");
    }
}

int go_to_ta(int student_id){
    
    int successful = 0;
    // Critical Section
    printf("Student %d is entering critical section \n", student_id);
    int shall_insert = sem_trywait(&sem_student);
    //printf("Shall_insert = %d \n", shall_insert);
    if (shall_insert == 0) successful = insert(student_id);
    sem_post(&sem_student);
    return successful;  // if he entered the queue

}


int insert(int student_id)
{
    int add_item;
    
    if (waiting_count == MAX)
    {    
        //printf("WAITING IS FULL, STUDENT %d is returning back\n", student_id);
        return 0;  // Addition Failed
    }
    else
    {
        
        if (front == - 1)  front = 0;  // If queue is empty
        printf("Student ID: %d is waiting for TA \n", student_id);
        rear = (rear + 1) % MAX_WAITING_STUDENTS;
        queue_chairs[rear] = student_id;
        
        if(waiting_count == 0) sem_post(&sem_ta);  // Wake up TA
        //pthread_mutex_lock(&mutex_waiting);
        waiting_count ++;
        //pthread_mutex_unlock(&mutex_waiting);
        //sem_wait(&student_waiting[rear]);
        return 1;  // Addition Successful
    }
    
    
}


void delete()
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("Student %d is sitting with TA\n", queue_chairs[front]);
    //sem_post(&student_waiting[front]);
    front = (front + 1) % MAX_WAITING_STUDENTS;
    waiting_count --;
    printf("Waiting Students = %d", waiting_count);
    
} /* End of delete() */


void sit_with_student(){
    delete();
}


void programming(int time){
    sleep(time);
}