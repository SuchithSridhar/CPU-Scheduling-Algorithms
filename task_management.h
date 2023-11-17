#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib/ssvector.h"

typedef struct {
    size_t id;
    char* taskname;
    long arrival;
    long burst;

    /* The following are required for the simulation. */
    long remaining_burst;
    long last_execution;
    long wait_time;
} Task;

typedef ss_Vector TaskList;

/**
 * Initialize a task with the given parameters while copying the
 * taskname string to the given task using strdup.
 */
bool task_init(Task *task, size_t id, char *taskname, long arrival, long burst);

/**
 * Create and initialize a task on the heap and return
 * a pointer to it.
 */
Task* task_create(size_t id, char *taskname, long arrival, long burst);

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
 * Get the size of the task list.
 */
size_t tasklist_size(TaskList *list);

/**
 * Check if the task list if empty.
 */
bool tasklist_empty(TaskList *list);

/**
 * Push a copy of a task onto the list.
 * If copy is true, a copy of the task is made,
 * else the provided task is used.
 */
bool tasklist_push(TaskList *list, Task *task, bool copy);

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
 * Pop a task at the specified index and return it.
 * Returns a pointer to the popped task. Task exists
 * on heap but not in list.
 */
Task* tasklist_pop_at(TaskList *list, size_t index);

/**
 * Create a deep copy of a given task list
 * and return a pointer to the copy.
 */
TaskList* tasklist_deep_copy(TaskList *list);

int tasklist_compare_id(const void *task_1, const void *task_2);
int tasklist_compare_burst(const void *task_1, const void *task_2);
int tasklist_compare_arrival(const void *task_1, const void *task_2);
int tasklist_compare_remaining_burst(const void *task_1, const void *task_2);

bool tasklist_sort(TaskList *list, int (*compare)(const void*, const void*));

/**
 * Read tasklist from a given file.
 */
TaskList* tasklist_from_file(FILE *file);

/**
 * Print a given task list.
 * the print function is called on each element.
 */
void tasklist_print(TaskList *list, void (*print)(void*));

/**
 * Check if a task needs to be added to the queue
 * because it has "arrived". If so, add this task
 * to the queue.
 * Returns number of tasks added to the queue.
 */
size_t task_process_arrival(TaskList *list, TaskList *queue, long cpu_clock);

#endif
