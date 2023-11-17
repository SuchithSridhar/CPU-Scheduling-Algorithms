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
 * Destroy a given tasklist and free all tasks it holds.
 */
void tasklist_destory(TaskList *list);

/**
 * Push a copy of a task onto the list.
 */
bool tasklist_push(TaskList *list, Task *task);

/**
 * Get the task at the front of the list.
 * Returns a pointer to the task as the front of the list.
 */
Task* tasklist_peek(TaskList *list);

/**
 * Get a task at a particular index.
 * Returns a pointer to the task at that index.
 */
Task* tasklist_get(TaskList *list, size_t index);

/**
 * Delete a task at the specified index. 
 * Returns true if successful, else false.
 */
bool tasklist_delete_at(TaskList *list, size_t index);

/**
 * Read tasklist from a given file.
 */
TaskList* tasklist_from_file(char *filename);

/**
 * Print a given task list.
 */
void tasklist_print(TaskList *list);

/**
 * Check if a task needs to be added to the queue
 * because it has "arrived". If so, add this task
 * to the queue.
 * Returns number of tasks added to the queue.
 */
size_t task_process_arrival(TaskList *list, TaskList *queue, long cpu_clock);

#endif
