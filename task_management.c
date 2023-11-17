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
    TaskList *list = malloc(sizeof(TaskList));
    if (list == NULL) return NULL;
    
    list->array = malloc(sizeof(Task) * TASKLIST_INIT_SIZE);
    if (list->array == NULL) return NULL;

    list->capacity = TASKLIST_INIT_SIZE;
    list->size = 0;
    return list;
}

bool tasklist_grow(TaskList *list) {
    if (list == NULL) return false;

    size_t new_capacity = (TASKLIST_GROW_FACTOR * list->capacity * sizeof(Task));
    Task *arr = list->array;
    Task *new = realloc(arr, new_capacity);

    if (new == NULL) return false;
    list->array = new;
    list->capacity = new_capacity;
    return true;
}

void tasklist_destory(TaskList *list) {
    if (list == NULL) return;
    for (int i = 0; i < list->size; i++) {
        free(list->array[i].taskname);
    }
    free(list);
}

TaskList* tasklist_from_file(char *filename) {

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *token;

    char *taskname;
    long arrival;
    long burst;

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

        taskname = token;

        token = strtok(NULL, ",");
        if (token == NULL) {
            tasklist_destory(list);
            fprintf(stderr, "Invalid file format.\n");
            exit(EXIT_FAILURE);
        }
        arrival = atol(token);

        token = strtok(NULL, ",");
        if (token == NULL) {
            tasklist_destory(list);
            fprintf(stderr, "Invalid file format.\n");
            exit(EXIT_FAILURE);
        }
        burst = atol(token);

        task_init(list->array + list->size, taskname, arrival, burst);
        list->size++;
        if (list->size == list->capacity) {
            bool success = tasklist_grow(list);
            if (!success) {
                tasklist_destory(list);
                return NULL;
            }
        }
    }

    fclose(fp);
    if (line) free(line);

    return list;
}

