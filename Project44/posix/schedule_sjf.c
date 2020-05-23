/******************************************************
> Author       : HaolinLiu 
> Last modified: 2020-03-22 19:39
> Email        : 25915043@qq.com
> Filename     : schedule_sjf.c
> Description  : SJF 
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
void schedule() {
    Task *shortest_task = task_list->task;
    struct node *tmp = task_list;

    while(task_list->next != NULL){
        tmp = task_list;
        shortest_task = task_list->task;

        while(tmp != NULL) {  
            if (tmp->task->burst < shortest_task->burst) {
                shortest_task = tmp->task;
            }
            tmp=tmp->next;
        }

        run(shortest_task, shortest_task->burst);
        delete(&task_list,shortest_task);
    }
    run(task_list->task,task_list->task->burst);

}
