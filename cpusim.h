#ifndef CPUSIM_H
#define CPUSIM_H

#include <stdbool.h>
#include "task_management.h"
#include "schedulers.h"

#define DEFAULT_OUTPUT_FILE "Output.txt"
#define DEFAULT_INPUT_FILE "TaskSpec.txt"

/**
 * Schedule a task using the provided scheduler.
 * Return true if a task switch took place, else false.
 */
bool task_schedule(SchedulerFunction scheduler, SchedulerArgs *args, long cpu_tick);

/**
 * Simulate a given scheduler with a given tasklist.
 */
void simulate_scheduler(SchedulerFunction scheduler, TaskList *list);
 
#endif
