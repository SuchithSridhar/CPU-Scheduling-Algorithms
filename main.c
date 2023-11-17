#include "cpusim.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "task_management.h"

void _task_print(void *task) {
    if (!task) return;
    Task *t = *((Task **) task);
    if (!t) return;
    printf("%4s: %7ld, %7ld\n", t->taskname, t->arrival, t->burst);
}

void _task_print_verbose(void *task) {
    if (!task) return;
    Task *t = *((Task **) task);
    if (!t) return;
    printf("Task %s\n", t->taskname);
    printf("Arrival: %ld, Burst: %ld\n", t->arrival, t->burst);
    printf("Waiting Time: %ld\n\n", t->wait_time);
}

bool fcfs_scheduler(Task **running_task, TaskList *queue) {
    // False if pointer is null or task already running.
    if (!running_task || (*running_task)) return false;

    *running_task = tasklist_pop_at(queue, 0);
    if (*running_task) return true;
    else return false;
}

bool task_schedule(SchedulerFunction scheduler, Task **run, TaskList *queue) {
    return scheduler(run, queue);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invalid call to program.\n");
        fprintf(stderr, "Please provide a file to read tasks from.\n");
        fprintf(stderr, "./program <filename>\n");
        return EXIT_FAILURE;
    }

    char *taskfile = argv[1];
    TaskList *tasklist = tasklist_from_file(taskfile);

    if (!tasklist) {
        fprintf(stderr, "Failed to create task list from provided file.\n");
        fprintf(stderr, "Please verify file structure.\n");
        return EXIT_FAILURE;
    }

    TaskList *queue = tasklist_create();
    TaskList *completed = tasklist_create();
    Task *running_task = NULL;
    bool task_scheduled;

    /* Count number of context switches. */
    long context_switch_counter = 0;
    long cpu_clock = 0;

    while (!tasklist_empty(tasklist) || !tasklist_empty(queue) || running_task) {
        // Moves tasks from tasklist to queue when they arrive.

        task_process_arrival(tasklist, queue, cpu_clock);

        task_scheduled = task_schedule(fcfs_scheduler, &running_task, queue);

        if (running_task) {
            // A task switch took place
            if (task_scheduled) {
                context_switch_counter++;
                running_task->wait_time += (
                    cpu_clock - running_task->last_execution
                );
            }
            running_task->remaining_burst--;
            running_task->last_execution = cpu_clock;
            if (running_task->remaining_burst == 0) {
                tasklist_push(completed, running_task, false);
                running_task = NULL;
            }
        }

        cpu_clock++;
    }

    tasklist_print(completed, _task_print_verbose);
    tasklist_destory(tasklist);

    return EXIT_SUCCESS;
}
