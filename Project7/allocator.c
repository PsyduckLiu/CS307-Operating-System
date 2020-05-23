/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-05-23 23:33
> Email        : 25915043@qq.com
> Filename     : allocator.c
> Description  : a contiguous memory allocation simulator 
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define NUMBER_OF_BLOCKS 1000
#define true 1
#define MAX_COMMAND_LENGTH 50
char *EMPTY_BLOCK="Unused";

int max;
int mem_block[NUMBER_OF_BLOCKS];
char *block_name[NUMBER_OF_BLOCKS];
int block_num;
char cmd_buffer[MAX_COMMAND_LENGTH];

void request(char* name, int size, char* strategy)
{
    int i,j;
    int index;
    int best_size;
    int worst_size;
    
    //first fit
    if (strategy[0]=='F') {
        for (i=0;i<block_num;i++) {
            if (strcmp(block_name[i],EMPTY_BLOCK)==0) {
                if (size<mem_block[i]) {
                    for (j=block_num;j>i;j--) {
                        block_name[j]=block_name[j-1];
                        mem_block[j]=mem_block[j-1];
                    }
                    mem_block[i+1]=mem_block[i]-size;

                    mem_block[i]=size;
                    block_name[i]=malloc(sizeof(char)*strlen(name));
                    strcpy(block_name[i],name);
                    block_num++;

                    break;
                }

                if (size==mem_block[i]) {
                    block_name[i]=malloc(sizeof(char)*strlen(name));
                    strcpy(block_name[i],name);

                    break;
                }
            }
        }

        if (i==block_num) {
            printf("There is insufficient memory to allocate to a request and reject the request.\n");
            return;
        }
    }

    //best fit
    if (strategy[0]=='B') {
        index=-1;
        best_size=max+1;

        for (i=0;i<block_num;i++) {
            if (strcmp(block_name[i],EMPTY_BLOCK)==0) {
                if (mem_block[i]>=size && mem_block[i]<best_size){
                    index=i;
                    best_size=mem_block[i];
                }
            }
        }
        if (index==-1) {
            printf("There is insufficient memory to allocate to a request and reject the request.\n");
            return;
        }

        if (size<best_size) {
            for (j=block_num;j>index;j--) {
                block_name[j]=block_name[j-1];
                mem_block[j]=mem_block[j-1];
            }
            mem_block[index+1]=mem_block[index]-size;

            mem_block[index]=size;
            block_name[index]=malloc(sizeof(char)*strlen(name));
            strcpy(block_name[index],name);
            block_num++;
        }

        if (size==best_size) {
            block_name[index]=malloc(sizeof(char)*strlen(name));
            strcpy(block_name[index],name);
        }
    }

    //worst fit
    if (strategy[0]=='W') {
        index=-1;
        worst_size=size-1;

        for (i=0;i<block_num;i++) {
            if (strcmp(block_name[i],EMPTY_BLOCK)==0) {
                if (mem_block[i]>=size && mem_block[i]>worst_size){
                    index=i;
                    worst_size=mem_block[i];
                }
            }
        }
        if (index==-1) {
            printf("There is insufficient memory to allocate to a request and reject the request.\n");
            return;
        }

        if (size<worst_size) {
            for (j=block_num;j>index;j--) {
                block_name[j]=block_name[j-1];
                mem_block[j]=mem_block[j-1];
            }
            mem_block[index+1]=mem_block[index]-size;

            mem_block[index]=size;
            block_name[index]=malloc(sizeof(char)*strlen(name));
            strcpy(block_name[index],name);
            block_num++;
        }

        if (size==worst_size) {
            block_name[index]=malloc(sizeof(char)*strlen(name));
            strcpy(block_name[index],name);
        }
    }

}

void release(char* name)
{
    int i,j;

    for (i=0;i<block_num;i++) {
        if (strcmp(block_name[i],name)==0) {
        strcpy(block_name[i],EMPTY_BLOCK);
        
        if (i>=1 && strcmp(block_name[i-1],EMPTY_BLOCK)==0) {
            mem_block[i-1]+=mem_block[i];

            for (j=i;j<block_num-1;j++) {
                block_name[j]=block_name[j+1];
                mem_block[j]=mem_block[j+1];
            }
            
            block_num--;
            i--;
        }

        if (i<block_num-1 && strcmp(block_name[i+1],EMPTY_BLOCK)==0) {
            mem_block[i]+=mem_block[i+1];

            for (j=i+1;j<block_num-1;j++) {
                block_name[j]=block_name[j+1];
                mem_block[j]=mem_block[j+1];
            }
                
            block_num--;
        }

        break;
        }
    }

    if (i==block_num) {
        printf("There is no Process %s in the memory.\n",name);
        return;
    }
}

void compact()
{
    int i;
    int index;
    int tmp_size;

    for (i=0;i<block_num;i++) {
        if (strcmp(block_name[i],EMPTY_BLOCK)!=0) {
            index=i;

            while(index>0 && strcmp(block_name[index-1],EMPTY_BLOCK)==0) {
                strcpy(block_name[index-1],block_name[index]);
                strcpy(block_name[index],EMPTY_BLOCK);

                tmp_size=mem_block[index-1];
                mem_block[index-1]=mem_block[index];
                mem_block[index]=tmp_size;

                index--;
            }
        }
    }

    for (i=0;i<block_num;i++) {
        if (strcmp(block_name[i],EMPTY_BLOCK)==0) {
            index=i;
            tmp_size=0;

            while(index<block_num) {
                tmp_size+=mem_block[index];
                index++;
            }

            mem_block[i]=tmp_size;
            block_num=i+1;

            break;
        }
    }
}

void print_all()
{
    int index=0;
    int i;

    for (i=0;i<block_num;i++) {
        if (strcmp(block_name[i],EMPTY_BLOCK)==0) {
            printf("Addresses [%d:%d] Unused\n",index,index+mem_block[i]-1);
        }
        else {
            printf("Addresses [%d:%d] Process %s\n",index,index+mem_block[i]-1,block_name[i]);
        }

        index=index+mem_block[i];
    }

}

int main(int argc, char *argv[])
{
    int i;
    char *p;
    char *name;
    int size;
    char *strategy;

    max=atoi(argv[1]);

    for (i=0;i<NUMBER_OF_BLOCKS;i++) {
        mem_block[i]=0;
    }
    mem_block[0]=max;
    block_name[0]=EMPTY_BLOCK;
    block_num=1;

    while(true) {
        printf("allocator>");
        fgets(cmd_buffer,MAX_COMMAND_LENGTH,stdin);
        p=strtok(cmd_buffer," ");

        if (cmd_buffer[0]=='X') {
            printf("Exit!\n");
            break;
        }
        if (cmd_buffer[1]=='Q') {
            name=strtok(NULL," ");
            size=atoi(strtok(NULL," "));
            strategy=strtok(NULL,"\n");
            request(name,size,strategy);
        }
        if (cmd_buffer[1]=='L') {
            name=strtok(NULL,"\n");
            release(name);
        }
        if (cmd_buffer[0]=='C') {
            compact();
        }
        if (cmd_buffer[0]=='S') {
            print_all();
        }
    }

    return 0;
}
