
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
    int current_thread=thread_count++;
    int low=current_thread*(num_of_array/2);
    int high=(current_thread+1)*(num_of_array/2)-1;
    int i=0,j=0;

    for (i=low;i<high;i++) {
        int tmp=i;

        for (j=i+1;j<=high;j++) {
            if (array[j]<array[tmp])
                tmp=j;
        }

        int temp=array[tmp];
        array[tmp]=array[i];
        array[i]=temp;
    }
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
}

int main()
{
    int i=0;

    printf("Please enter the number of array elements:\n");
    scanf("%d",&num_of_array );

    array=malloc(num_of_array*sizeof(int));
    result=malloc(num_of_array*sizeof(int));
    
    printf("Please enter the array elements:\n");
    for (i=0;i<num_of_array;i++) {
        scanf("%d",&array[i]);
    }

    pthread_t sort_thread[2];
    for (i=0;i<2;i++) {
        if (pthread_create(&sort_thread[i],NULL,sort,(void*)NULL)!=0)
            printf("Thread create error!");
    }

    for (i=0;i<2;i++) 
        pthread_join(sort_thread[i],NULL);
/*
    for (i=0;i<num_of_array;i++) printf("%d",array[i]);
    printf("\n");
*/
    pthread_t merge_thread;
    if (pthread_create(&merge_thread,NULL,merge,NULL)!=0)
        printf("Thread create error!");
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
