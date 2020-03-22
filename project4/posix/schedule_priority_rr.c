/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-22 19:40
> Email        : 25915043@qq.com
> Filename     : schedule_priority_rr.c
> Description  : Priority Round Robin 
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *task_list[MAX_PRIORITY+1];

// add a task to the list
void add(char *name, int priority, int burst) {
    Task *new_task;
            
    new_task = malloc(sizeof(Task));
                
    new_task->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(new_task->name, name);
    new_task->priority = priority;
    new_task->burst = burst;

    insert(&task_list[priority], new_task);
}

// invoke the scheduler
void schedule() {
    int i;
    int slice;
    struct node *tmp = NULL;
    struct node *new = NULL;
    Task *running_task = NULL;
    
    for (i = MAX_PRIORITY;i >= MIN_PRIORITY;i--) {
        if (task_list[i] != NULL) {
        new = NULL;
        tmp = task_list[i]; 

        while (tmp->next != NULL) {
            insert(&new,tmp->task );
            tmp = tmp->next;
                                
        }
        insert(&new,tmp->task);
        task_list[i] = new;

        tmp = task_list[i];

        while (task_list[i] != NULL) {
            running_task = tmp->task;

            if (tmp->next != NULL)
                tmp = tmp->next;
            else tmp = task_list[i];
                            
            if (QUANTUM < running_task->burst) 
                slice = QUANTUM;
            else
                slice = running_task->burst;
        
            run(running_task,slice);

            running_task->burst -= slice;

            if (running_task->burst == 0)
                delete(&task_list[i],running_task);
        }
    }
        
}
}
