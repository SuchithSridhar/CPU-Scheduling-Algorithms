#include "task_management.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include "lib/ssvector.h"

bool task_init(Task *task, size_t id, char *taskname, long arrival, long burst) {
    if (task == NULL) false;

    task->taskname = strdup(taskname);
    if (!task->taskname) return false;

    task->id = id;
    task->arrival = arrival;
    task->burst = burst;
    task->remaining_burst = burst;
    task->last_execution = arrival - 1;
    task->wait_time = 0;

    return true;
}

Task* task_create(size_t id, char *taskname, long arrival, long burst) {
    Task *task = malloc(sizeof(Task));
    if (task == NULL) return NULL;

    bool success = task_init(task, id, taskname, arrival, burst);
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

size_t tasklist_size(TaskList *list) {
    if (list) return list->size;
    return 0;
}

bool tasklist_empty(TaskList *list) {
    if (list) return list->size == 0;
    return true;
}

bool tasklist_push(TaskList *list, Task *task, bool copy) {
    if (!list || !task) return false;

    Task *t;

    if (copy) {
        t = task_create(task->id, task->taskname, task->arrival, task->burst);
        if (!t) return false;
    } else {
        t = task;
    }

    bool success = ssv_push(list, &t);
    if (!success) {
        task_destroy(t);
        return false;
    }

    return true;
}

void tasklist_print(TaskList *list, void (*print)(void*)) {
    if (!list) return;
    ssv_print(list, print);
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

Task* tasklist_pop_at(TaskList *list, size_t index) {
    Task **task_ptr = ssv_get(list, index);
    if (!task_ptr) return NULL;

    Task *ret_task = *task_ptr;

    bool success = ssv_delete_at(list, index);
    if (!success) return NULL;

    return ret_task;
}

TaskList* tasklist_deep_copy(TaskList *list) {
    if (!list) return NULL;

    TaskList *copy = tasklist_create();
    if (!copy) return NULL;

    Task **array = list->array;
    for (size_t i = 0; i < list->size; i++) {
        tasklist_push(copy, array[i], true);
    }
    return copy;
}


int tasklist_compare_id(const void *task_1, const void *task_2) {
    long l = (*(Task**)task_1)->id;
    long r = (*(Task**)task_2)->id;
    return (l - r);
}

int tasklist_compare_burst(const void *task_1, const void *task_2) {
    long l = (*(Task**)task_1)->burst;
    long r = (*(Task**)task_2)->burst;
    return (l - r);
}

int tasklist_compare_arrival(const void *task_1, const void *task_2) {
    long l = (*(Task**)task_1)->arrival;
    long r = (*(Task**)task_2)->arrival;
    return (l - r);
}

int tasklist_compare_remaining_burst(const void *task_1, const void *task_2) {
    long l = (*(Task**)task_1)->remaining_burst;
    long r = (*(Task**)task_2)->remaining_burst;
    return (l - r);
}

bool tasklist_sort(TaskList *list, int (*compare)(const void*, const void*)) {
    if (!list || !compare) return false;

    qsort(list->array, list->size, sizeof(Task*), compare);
    return true;
}

TaskList* tasklist_from_file(FILE *file) {

    if (!file) return NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *token;

    Task current_task;

    TaskList *list = tasklist_create();
    if (list == NULL) {
        return NULL;
    }

    current_task.id = 1;
    while ((read = getline(&line, &len, file)) != -1) {
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

        tasklist_push(list, &current_task, true);
        current_task.id++;
    }

    if (line) free(line);

    return list;
}

size_t task_process_arrival(TaskList *list, TaskList *queue, long cpu_clock) {
    Task *cur_task;
    Task *pop_task;
    size_t counter = 0;

    for (size_t i = 0; i < list->size; i++) {
        cur_task = tasklist_get(list, i);

        if (cur_task && cur_task->arrival <= cpu_clock) {
            pop_task = tasklist_pop_at(list, i);

            if (pop_task) {
                tasklist_push(queue, pop_task, false);
                counter++;
            }
        }
    }

    return counter;
}
