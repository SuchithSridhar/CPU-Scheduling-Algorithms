#include "cpusim.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "task_management.h"


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

    tasklist_print(tasklist);
    tasklist_destory(tasklist);

    return EXIT_SUCCESS;
}
