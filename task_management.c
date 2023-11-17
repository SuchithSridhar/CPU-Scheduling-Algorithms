#include "task_management.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "includes/ssvector.h"

bool task_init(Task *task, char *taskname, long arrival, long burst) {
    if (task == NULL) false;

    task->taskname = strdup(taskname);
    task->arrival = arrival;
    task->burst = burst;

    return true;
}

Task* task_create(char *taskname, long arrival, long burst) {
    Task *task = malloc(sizeof(Task));
    if (task == NULL) return NULL;

    bool success = task_init(task, taskname, arrival, burst);
    if (!success) {
        free(task);
        return NULL;
    }

    return task;
}

void task_destroy(Task *task) {
    if (task == NULL) return;
    free(task->taskname);
    free(task);
}

TaskList* tasklist_create() {
    ss_Vector *list = ssv_init(sizeof(Task*));
    if (list == NULL) return NULL;
    
    return list;
}

void tasklist_destory(TaskList *list) {
    if (list == NULL) return;
    Task **array = list->array;
    for (int i = 0; i < list->size; i++) {
        task_destroy(array[i]);
    }
    ssv_destroy(list);
}

bool tasklist_push(TaskList *list, Task *task) {
    if (!list || !task) return false;

    Task *t_copy = task_create(task->taskname, task->arrival, task->burst);
    if (!t_copy) return false;

    bool success = ssv_push(list, &t_copy);
    if (!success) {
        task_destroy(t_copy);
        return false;
    }

    return true;
}

void _task_print(void *task) {
    if (!task) return;
    Task *t = *((Task **) task);
    printf("%4s: %7ld, %7ld\n", t->taskname, t->arrival, t->burst);
}

void tasklist_print(TaskList *list) {
    if (!list) return;
    printf("%4s: %7s, %7s\n", "Name", "Arrival", "Burst");
    ssv_print(list, _task_print);
}

Task* tasklist_peek(TaskList *list) {
    Task **task_ptr = ssv_peek(list);
    if (!task_ptr) return NULL;
    else return *task_ptr;
}

Task* tasklist_get(TaskList *list, size_t index) {
    Task **task_ptr = ssv_get(list, index);
    if (!task_ptr) return NULL;
    else return *task_ptr;
}

bool tasklist_delete_at(TaskList *list, size_t index) {
    Task **task_ptr = ssv_get(list, index);
    if (!task_ptr) return false;

    bool success = ssv_delete_at(list, index);
    if (!success) return false;

    task_destroy(*task_ptr);

    return true;
}

TaskList* tasklist_from_file(char *filename) {

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *token;

    Task current_task;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Unable to find specified file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    TaskList *list = tasklist_create();
    if (list == NULL) {
        fclose(fp);
        return NULL;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        token = strtok(line, ",");
        if (token == NULL) {
            tasklist_destory(list);
            fprintf(stderr, "Invalid file format.\n");
            exit(EXIT_FAILURE);
        }

        current_task.taskname = token;

        token = strtok(NULL, ",");
        if (token == NULL) {
            tasklist_destory(list);
            fprintf(stderr, "Invalid file format.\n");
            exit(EXIT_FAILURE);
        }
        current_task.arrival = atol(token);

        token = strtok(NULL, ",");
        if (token == NULL) {
            tasklist_destory(list);
            fprintf(stderr, "Invalid file format.\n");
            exit(EXIT_FAILURE);
        }
        current_task.burst = atol(token);

        tasklist_push(list, &current_task);
    }

    fclose(fp);
    if (line) free(line);

    return list;
}

size_t task_process_arrival(TaskList *list, TaskList *queue, long cpu_clock) {
    Task *array = list->array;
    Task *cur_task;
    size_t counter = 0;

    for (size_t i = 0; i < list->size; i++) {
        cur_task = tasklist_get(list, i);
        if (cur_task && cur_task->arrival <= cpu_clock) {
            tasklist_push(queue, cur_task);
            counter++;
        }
    }

    return counter;
}
