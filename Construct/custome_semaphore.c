#include <stdio.h>
#include <pthread.h>

/* create a own semaphore and you have an mutex avalianle*/
typedef struct semaphore_t
{
    int sem_cap;
    pthread_mutex_t mutex;
}semaphore_t;

int sem_init(semaphore_t *sem,int init)
{
    sem->sem_cap = init;
    sem->mutex = PTHREAD_MUTEX_INITIALIZER;
}


void sem_wait(semaphore_t *sem)
{
    while(sem->sem_cap <= 0);//busy-wait
    pthread_mutex_lock(&mutex);
    sem->sem_cap--;
    pthread_mutex_unlock(&mutex);
}
void sem_post(semaphore_t *sem)
{
    pthread_mutex_lock(&mutex);
    sem->sem_cap++;
    pthread_mutex_unlock(&mutex);
}
