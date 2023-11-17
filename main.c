#include "cpusim.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "task_management.h"
#include "schedulers.h"

bool DEBUG = false;
FILE *OUTPUT_STREAM;

/**
 * Function used to print only when debug flag is set to true.
 */
void printlog(const char* format, ...) {
    if (!DEBUG) return;
    va_list args;
    va_start(args, format);
    vfprintf(OUTPUT_STREAM, format, args);
    va_end(args);
}

void _task_print_verbose(void *task_ptr) {
    if (!task_ptr) return;
    Task *t = *((Task **) task_ptr);
    if (!t) return;
    printlog("Task %s (%ld) \n", t->taskname, t->id);
    printlog("Arrival: %ld, Burst: %ld\n", t->arrival, t->burst);
    printlog("Waiting: %ld, Rem_Burst: %ld\n", t->wait_time, t->remaining_burst);
    printlog("Last Execution: %ld\n", t->last_execution);
    printlog("\n");
}

void task_print_burst(char *taskname, long prev_start, long cpu_tick) {
    fprintf(OUTPUT_STREAM, "%s\t%ld\t%ld\n", taskname, prev_start, cpu_tick);
}

void _task_print_waiting(void *task_ptr) {
    if (!task_ptr) return;
    Task *t = *((Task **) task_ptr);
    if (!t) return;
    fprintf(OUTPUT_STREAM, "Waiting Time %s: %ld\n", t->taskname, t->wait_time);
}


bool task_schedule(SchedulerFunction scheduler, SchedulerArgs *args, long cpu_tick) {
    return scheduler(args, cpu_tick);
}

double tasks_avg_wait_time(TaskList *completed) {
    if (!completed) return 0;
    Task **array = completed->array;
    double total = 0;
    for (size_t i = 0; i < completed->size; i++) {
        total += array[i]->wait_time;
    }

    return total / completed->size;
}

void simulate_scheduler(SchedulerFunction scheduler, TaskList *list) {
    
    TaskList *tasklist = tasklist_deep_copy(list);
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

    Task *rt, *prev_rt;
    long prev_start;

    while (!tasklist_empty(tasklist) || !tasklist_empty(queue) || sargs->running_task) {
        
        task_process_arrival(tasklist, queue, cpu_clock);

        prev_rt = sargs->running_task;
        prev_start = sargs->task_start;

        task_scheduled = task_schedule(scheduler, sargs, cpu_clock);

        rt = sargs->running_task;

        if (rt) {

            // A task switch took place
            if (task_scheduled && prev_rt) {
                task_print_burst(prev_rt->taskname, prev_start, cpu_clock);
            }

            // New task scheduled
            if (task_scheduled) {
                context_switch_counter++;
                sargs->task_start = cpu_clock;
                rt->wait_time += (
                    cpu_clock - rt->last_execution - 1
                );
            }
            rt->remaining_burst--;
            rt->last_execution = cpu_clock;

            if (rt->remaining_burst == 0) {
                tasklist_push(completed, rt, false);
                sargs->running_task = NULL;
                task_print_burst(rt->taskname, sargs->task_start, cpu_clock+1);
            }
        }

        cpu_clock++;
    }

    tasklist_sort(completed, tasklist_compare_id);
    tasklist_print(completed, _task_print_waiting);

    fprintf(OUTPUT_STREAM, "Average Waiting Time: %.2lf\n", 
            tasks_avg_wait_time(completed));

    tasklist_destory(tasklist);
    tasklist_destory(queue);
    tasklist_destory(completed);
    free(sargs);
}

int main(int argc, char *argv[]) {

    char* input_file = NULL;
    char* output_file = NULL;

    if (argc == 1) {
        // No CLI arguments provided
        input_file = DEFAULT_INPUT_FILE;
        output_file = DEFAULT_OUTPUT_FILE;

    } else if (argc == 3) {
        // ./program <inputfile> <outputfile>
        // if <inputfile> == stdin then standard in is used.
        // if <outputfile> == stdout then standard out if used.
        input_file = strcmp(argv[1], "stdin") == 0 ? NULL : argv[1];
        output_file = strcmp(argv[2], "stdout") == 0 ? NULL : argv[2];

    } else if (argc == 4) {
        // ./program -v <inputfile> <outputfile>
        if (strcmp(argv[1], "-v") != 0) {
            fprintf(stderr, "Invalid call to program.\n");
            fprintf(stderr, "Exiting.\n");
            return EXIT_FAILURE;
        }
        input_file = strcmp(argv[2], "stdin") == 0 ? NULL : argv[2];
        output_file = strcmp(argv[3], "stdout") == 0 ? NULL : argv[3];

    } else {
        fprintf(stderr, "Invalid call to program.\n");
        fprintf(stderr, "Exiting.\n");
        return EXIT_FAILURE;
    }

    TaskList *tasklist;

    if (input_file == NULL) {
        tasklist = tasklist_from_file(stdin);
    } else {
        FILE *fio = fopen(input_file, "r");
        if (!fio) {
            fprintf(stderr, "Unable to read file %s.\n", input_file);
            fprintf(stderr, "Exiting.\n");
            return EXIT_FAILURE;
        }
        tasklist = tasklist_from_file(fio);
        fclose(fio);
    }

    if (!tasklist) {
        fprintf(stderr, "Failed to create task list from provided file.\n");
        fprintf(stderr, "Please verify file structure.\n");
        return EXIT_FAILURE;
    }

    SchedulerFunction schedulers[] = {
        fcfs_scheduler,
        rr_scheduler,
        nsjf_scheduler,
        psjf_scheduler
    };
    char *titles[] = {"FCFS", "RR", "NSJF", "PSJF"};
    size_t scheduler_count = sizeof(schedulers)  / sizeof(SchedulerFunction);

    if (output_file == NULL) {
        OUTPUT_STREAM = stdout;
    } else {
        OUTPUT_STREAM = fopen(output_file, "w");
        if (!output_file) {
            fprintf(stderr, "Unable to write file %s.\n", output_file);
            fprintf(stderr, "Exiting.\n");
            tasklist_destory(tasklist);
            return EXIT_FAILURE;
        }
    }

    for (size_t i = 0; i < scheduler_count; i++) {
        fprintf(OUTPUT_STREAM, "%s:\n", titles[i]);
        simulate_scheduler(schedulers[i], tasklist);
        if (i != scheduler_count - 1) fprintf(OUTPUT_STREAM, "\n");
    }

    if (output_file != NULL) {
        fclose(OUTPUT_STREAM);
    }

    tasklist_destory(tasklist);

    return EXIT_SUCCESS;
}
