/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-04-05 22:38
> Email        : 25915043@qq.com
> Filename     : Multithread.c
> Description  : multithread
*******************************************************/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int *array;
int *result;
int num_of_array;
int thread_count=0;

void *sort(void *arg)
{
    int low;
    int high;
    int i=0,j=0,temp=0,tmp=0;
	
	if (thread_count==0) {
		low=0;
		high=(thread_count+1)*(num_of_array/2);
	}
	if (thread_count==1) {
		low=(thread_count)*(num_of_array/2);
		high=num_of_array;
	}
    for (i=low;i<high;i++) {
        tmp=i;

        for (j=i+1;j<high;j++) {
            if (array[j]<array[tmp])
                tmp=j;
        }

        temp=array[tmp];
        array[tmp]=array[i];
        array[i]=temp;
    }
	
	thread_count++;
	
	pthread_exit(0);
}

void *merge(void *arg)
{
    int index1=0;
    int index2=num_of_array/2;
    int i=0;

    for (i=0;i<num_of_array;i++) {
        if (index1<num_of_array/2 && index2<num_of_array) {
            if (array[index1]<array[index2])
                result[i]=array[index1++];
            else result[i]=array[index2++];
        }
        else {
            if (index1<num_of_array/2) result[i]=array[index1++];
            else result[i]=array[index2++];
        }
    }
	
	pthread_exit(0);
}

int main()
{
    int i=0;
	pthread_t sort_thread[2];
	pthread_t merge_thread;
	pthread_attr_t attr;
	
    printf("Please enter the number of array elements:\n");
    scanf("%d",&num_of_array);

    array=malloc(num_of_array*sizeof(int));
    result=malloc(num_of_array*sizeof(int));
    
    printf("Please enter the array elements:\n");
    for (i=0;i<num_of_array;i++) {
        scanf("%d",&array[i]);
    }

    for (i=0;i<2;i++) {
		pthread_attr_init(&attr);
        pthread_create(&sort_thread[i],&attr,sort,NULL);
    }

    for (i=0;i<2;i++) 
        pthread_join(sort_thread[i],NULL);

	pthread_attr_init(&attr);
    pthread_create(&merge_thread,&attr,merge,NULL);
    pthread_join(merge_thread,NULL);
    
    printf("The sorted array is:\n");
    for (i=0;i<num_of_array;i++) {
        printf("%d",result[i]);
        printf(" ");
    }
    printf("\n");
    free(array);
    free(result);

    return 0;
}
