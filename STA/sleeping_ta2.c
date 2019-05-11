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
void programming();



pthread_t STUDENTS[MAX_STUDENTS];
pthread_t TA;

sem_t *sem_student;  // for accessing the chair
sem_t *sem_ta;  // for waking up the TA
pthread_mutex_t *mutex_waiting;  // for locking the  count

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
    pthread_mutex_init(mutex_waiting, NULL);
    sem_init(sem_ta, 0, 0);  // Initially TA is sleeping
    sem_init(sem_student, 0, 0); // Initially asking for a chair is possible

    pthread_join(&TA, NULL);
    for (int i = 0; i < MAX_STUDENTS; i++)
    {
        student_ids[i] = i;
        pthread_join(STUDENTS[i], (void *) student_ids + i);
    }
    sem_destroy(sem_ta);
    sem_destroy(sem_student);
    return 0;
}

void *simulate_student(void * param){
    int shall_go_to_ta;
    int student_id = *((int *)param);
    printf("Created Thread for student: %d\n", student_id);

    while(1){
        shall_go_to_ta = rand()%10;
        if (shall_go_to_ta < 4)
            if (!go_to_ta(student_id)) programming();
    }
}


void *simulate_ta(void *param){
    printf("Created Thread for TA\n");
    while(1){
        pthread_mutex_lock(mutex_waiting);
        if(waiting_count) sit_with_student();
        pthread_mutex_unlock(mutex_waiting);
        sem_wait(sem_ta); // Sleep or busy waiting until a student come
    }
}
int go_to_ta(int student_id){
    
    int successful = 0;
    // Critical Section
    int shall_insert = sem_trywait(sem_student);
    if (shall_insert == 0) successful = insert(student_id);
    sem_post(sem_student);
    return successful;  // if he entered the queue

}


int insert(int student_id)
{
    int add_item;
    if (rear == MAX - 1)
        return 0;  // Addition Failed
    else
    {
        if (front == - 1)  front = 0;  // If queue is empty
        printf("Student ID: %d is waiting for TA ", student_id);
        rear = (rear + 1) % MAX_WAITING_STUDENTS;
        queue_chairs[rear] = student_id;
        
        pthread_mutex_lock(mutex_waiting);
        if(waiting_count == 0) sem_post(sem_ta);  // Wake up TA
        waiting_count ++;
        pthread_mutex_unlock(mutex_waiting);
        
        return 1;  // Addition Successful
    }
}


void delete()
{
    if (front == - 1 || front > rear)
    {
        printf("Queue Underflow \n");
        return ;
    }
    else
    {
        printf("Student %d is sitting with TA\n", queue_chairs[front]);
        front = (front + 1) % MAX_WAITING_STUDENTS;
        waiting_count --;
        
    }
} /* End of delete() */


void sit_with_student(){
    delete();
}


void programming(){
    sleep(5);
}