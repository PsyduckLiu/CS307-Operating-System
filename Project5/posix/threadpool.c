/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-18 23:29
> Email        : 25915043@qq.com
> Filename     : threadpool.c
> Description  : thread pool 
*******************************************************/

/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo[QUEUE_SIZE];
task tmp_task;
int task_num,shutdown;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

pthread_mutex_t mutex;
sem_t sem;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
    if (task_num==QUEUE_SIZE)
        return 1;

    worktodo[task_num]=t;
	task_num++;

    return 0; 
}

// remove a task from the queue
task dequeue() 
{
	task_num--;
	tmp_task = worktodo[task_num];
		
    return tmp_task;
}

// the worker thread in the thread pool
void *worker(void *param)
{
    while(TRUE)
    {
    	sem_wait(&sem);
        if(shutdown==1)
            pthread_exit(0);
	   
        pthread_mutex_lock(&mutex);

        tmp_task=dequeue();
        execute(tmp_task.function,tmp_task.data);
	
        pthread_mutex_unlock(&mutex);
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);	
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
	int flag;

    tmp_task.function=somefunction;
    tmp_task.data=p;

	pthread_mutex_lock(&mutex);
	
    flag=enqueue(tmp_task);
    if (flag==0)
        sem_post(&sem);
	
    pthread_mutex_unlock(&mutex);
    
    return flag;
}

// initialize the thread pool
void pool_init(void)
{
	task_num=0;
	shutdown=0;
	
	pthread_mutex_init(&mutex,NULL);
	sem_init(&sem,0,0);
	for(int i=0;i<NUMBER_OF_THREADS;i++) {
        pthread_create(&bee[i],NULL,worker,NULL);
		printf("Create threads sucessfully!\n");
	}
}

// shutdown the thread pool
void pool_shutdown(void)
{
	shutdown=1;
	
    for(int i=0;i<NUMBER_OF_THREADS;i++)
		sem_post(&sem);
	for(int i=0;i<NUMBER_OF_THREADS;i++) {
		if (pthread_join(bee[i],NULL)==0)
        printf("Join threads successfully!\n");
	}
	
    pthread_mutex_destroy(&mutex);
	sem_destroy(&sem);
}
