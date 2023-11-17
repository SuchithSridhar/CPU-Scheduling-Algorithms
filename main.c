#include "cpusim.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "task_management.h"

void _task_print(void *task_ptr) {
    if (!task_ptr) return;
    Task *t = *((Task **) task_ptr);
    if (!t) return;
    printf("%4s: %7ld, %7ld\n", t->taskname, t->arrival, t->burst);
}

void _task_print_verbose(void *task_ptr) {
    if (!task_ptr) return;
    Task *t = *((Task **) task_ptr);
    if (!t) return;
    printf("Task %s (%ld) \n", t->taskname, t->id);
    printf("Arrival: %ld, Burst: %ld\n", t->arrival, t->burst);
    printf("Waiting Time: %ld\n\n", t->wait_time);
}

bool fcfs_scheduler(SchedulerArgs *args, long cpu_tick) {
    // False if pointer is null or task already running.
    if (!args || args->running_task) return false;

    if (tasklist_empty(args->queue)) return false;

    args->running_task = tasklist_pop_at(args->queue, 0);
    return args->running_task != NULL;
}

bool rr_scheduler(SchedulerArgs *args, long cpu_tick) {
    if (!args) return false;

    if (tasklist_empty(args->queue)) return false;

    if (!args->running_task) {
        // No currently running task
        args->running_task = tasklist_pop_at(args->queue, 0);

        if (!args->running_task) return false;

        args->task_start = cpu_tick;
        return true;

    } 

    if (cpu_tick - args->task_start == RR_TIME_SHARE) {
        // Time share complete and queue not empty
        Task *task = tasklist_pop_at(args->queue, 0);
        tasklist_push(args->queue, args->running_task, false);
        args->running_task = task;
        args->task_start = cpu_tick;
        return true;
    }

    return false;
}

bool task_schedule(SchedulerFunction scheduler, SchedulerArgs *args, long cpu_tick) {
    return scheduler(args, cpu_tick);
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
    bool task_scheduled;

    /* Count number of context switches. */
    long context_switch_counter = 0;
    long cpu_clock = 0;

    SchedulerArgs *sargs = malloc(sizeof(SchedulerArgs));
    sargs->running_task = NULL;
    sargs->queue = queue;
    sargs->task_start = 0;

    while (!tasklist_empty(tasklist) || !tasklist_empty(queue) || sargs->running_task) {
        // Moves tasks from tasklist to queue when they arrive.

        task_process_arrival(tasklist, queue, cpu_clock);

        task_scheduled = task_schedule(rr_scheduler, sargs, cpu_clock);

        if (sargs->running_task) {

            // A task switch took place
            if (task_scheduled) {
                printf("Switch to %ld at %ld.\n", sargs->running_task->id, cpu_clock);
                context_switch_counter++;
                sargs->running_task->wait_time += (
                    cpu_clock - sargs->running_task->last_execution - 1
                );
            }
            sargs->running_task->remaining_burst--;
            sargs->running_task->last_execution = cpu_clock;
            if (sargs->running_task->remaining_burst == 0) {
                tasklist_push(completed, sargs->running_task, false);
                sargs->running_task = NULL;
            }
        }

        cpu_clock++;
    }

    tasklist_sort(completed, tasklist_compare_id);
    tasklist_print(completed, _task_print_verbose);
    tasklist_destory(tasklist);
    tasklist_destory(completed);
    tasklist_destory(queue);
    free(sargs);

    return EXIT_SUCCESS;
}
