/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-19 18:34
> Email        : 25915043@qq.com
> Filename     : producer_and_consumer.c
> Description  : producer and consumer 
*******************************************************/

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TRUE 1

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

pthread_mutex_t mutex;
sem_t empty; 
sem_t full; 
int head,tail,turn;

int insert_item(buffer_item item) {
	/* insert item into buffer
	return 0 if successful, otherwise
	return -1 indicating an error condition */	
	if ((tail+1)%BUFFER_SIZE==head)
        	return -1;
	
	buffer[tail]=item;
	tail=(tail+1)%BUFFER_SIZE;

	return 0;
}

int remove_item(buffer_item *item) {
	/* remove an object from buffer
	placing it in item
	return 0 if successful, otherwise
	return -1 indicating an error condition */
	if (head==tail) 
        return -1;
 
    item=&buffer[head];
    head=(head+1)%BUFFER_SIZE;
    
	return 0;
}

void init()
{
    head=tail=turn=0;
	
    pthread_mutex_init(&mutex,NULL);
	sem_init(&full,0,0);
	sem_init(&empty,0,BUFFER_SIZE-1);
}

void *producer(void *param) {
    buffer_item item;
	
    while (TRUE) {
		/* sleep for a random period of time */
		sleep(rand()%3);
		/* generate a random number */
		item=rand();

		sem_wait(&empty);
		pthread_mutex_lock(&mutex);

		if (insert_item(item))
			printf("producer report error condition\n");
		else 
			printf("producer produced %d\n",item);

		pthread_mutex_unlock(&mutex);
		sem_post(&full);
    }
}

void *consumer(void *param) {
    buffer_item item;
   
	while (TRUE) {
		/* sleep for a random period of time */
		sleep(rand()%3);

		sem_wait(&full);
		pthread_mutex_lock(&mutex);

		if (remove_item(&item))
			printf("consumer report error condition\n");
		else
			printf("consumer consumed %d\n",buffer[(head+BUFFER_SIZE-1)%BUFFER_SIZE]);

		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
    }
}

int main(int argc, char const *argv[])
{
    /* 1. Get command line arguments argv[1],argv[2],argv[3] */
	/* 2. Initialize buffer */
	/* 3. Create producer thread(s) */
	/* 4. Create consumer thread(s) */
	/* 5. Sleep */
	/* 6. Exit */
    int time=0;
	int producer_num=0;
	int consumer_num=0;
	int i=0;
	
    if(argc!= 4) {
		printf("Invalid Input\n");
		return -1;
	}
    time=atoi(argv[1]);
	producer_num=atoi(argv[2]);
	consumer_num=atoi(argv[3]);

    init();

    pthread_t producer_thread[producer_num];
	pthread_t consumer_thread[consumer_num];

	for(i=0;i<producer_num;i++) {
		pthread_create(&producer_thread[i],NULL,&producer,NULL);
	}
	for(i=0;i<consumer_num;i++) {
		pthread_create(&consumer_thread[i],NULL,&consumer,NULL);
	}

	sleep(time);
    
	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);
 
    return 0;
}

