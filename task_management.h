#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H

#include <stdlib.h>
#include <stdbool.h>

#include "includes/ssvector.h"

typedef struct {
    char* taskname;
    long arrival;
    long burst;
} Task;

typedef ss_Vector TaskList;

/**
 * Initialize a task with the given parameters while copying the
 * taskname string to the given task using strdup.
 */
bool task_init(Task *task, char *taskname, long arrival, long burst);

/**
 * Create and initialize a task on the heap and return
 * a pointer to it.
 */
Task* task_create(char *taskname, long arrival, long burst);

/**
 * Destroy a task cleanly.
 * Frees the memory allocated for the taskname.
 */
void task_destroy(Task *task);

/**
 * Create list to hold tasks.
 * Created on the heap. Call tasklist_destroy to free.
 */
TaskList* tasklist_create();

/**
 * Grow task list when required.
 * Returns true if successful, else false.
 */
bool tasklist_grow(TaskList *list);

/**
 * Destroy a given tasklist and free all tasks it holds.
 */
void tasklist_destory(TaskList *list);

/**
 * Read tasklist from a given file.
 */
TaskList* tasklist_from_file(char *filename);

#endif
