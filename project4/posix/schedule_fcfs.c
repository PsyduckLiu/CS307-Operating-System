/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-22 19:38
> Email        : 25915043@qq.com
> Filename     : schedule_fcfs.c
> Description  : FCFS 
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *task_list = NULL;

// add a task to the list
void add(char *name, int priority, int burst) {
    Task *new_task;
	new_task = malloc(sizeof(Task));

    new_task->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(new_task->name, name);
    new_task->priority = priority;
    new_task->burst = burst;
   
    insert(&task_list, new_task);
}

// invoke the scheduler
void schedule(){
	struct node *last_node = task_list;

    while(task_list->next != NULL){
        last_node = task_list;
        
		//该链表的insert是从后往前insert的，所以最后一个非NULL节点的进程是第一个被插入的进程
		while(last_node->next != NULL)  
			last_node = last_node -> next;
        
		run(last_node->task, last_node->task->burst);
        delete(&task_list, last_node->task);
    }
    run(task_list->task, task_list->task->burst);
	
}
