#ifndef CPUSIM_H
#define CPUSIM_H

#include <stdbool.h>
#include "task_management.h"

#define RR_TIME_SHARE 4

typedef struct {
    Task *running_task;
    TaskList *queue;
    long task_start;
} SchedulerArgs;

typedef bool (*SchedulerFunction) (SchedulerArgs *args, long cpu_tick);

/**
 * Schedule a task using the provided scheduler.
 * Return true if a task switch took place, else false.
 */
bool task_schedule(SchedulerFunction scheduler, SchedulerArgs *args, long cpu_tick);

/**
 * First-come first-served task scheduler.
 */
bool fcfs_scheduler(SchedulerArgs *args, long cpu_tick);

/**
 * Round-Robin task scheduler.
 */
bool rr_scheduler(SchedulerArgs *args, long cpu_tick);

/**
 * Non-preemptive shortest-job-first task scheduler.
 */
bool nsjf_scheduler(SchedulerArgs *args, long cpu_tick);

/**
 * Preemptive shortest-job-first task scheduler.
 */
bool psjf_scheduler(SchedulerArgs *args, long cpu_tick);
 
#endif
