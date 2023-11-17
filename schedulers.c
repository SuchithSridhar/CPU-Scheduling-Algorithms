#include "schedulers.h"

#include <stdlib.h>
#include "task_management.h"

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

        return true;

    } 

    if (cpu_tick - args->task_start == RR_TIME_SHARE) {
        // Time share complete and queue not empty
        Task *task = tasklist_pop_at(args->queue, 0);
        tasklist_push(args->queue, args->running_task, false);
        args->running_task = task;
        return true;
    }

    return false;
}

bool nsjf_scheduler(SchedulerArgs *args, long cpu_tick) {
    if (!args) return false;

    if (tasklist_empty(args->queue)) return false;

    if (!args->running_task) {
        Task **array = args->queue->array;
        long min = array[0]->burst;
        size_t task_index = 0;
        for (size_t i = 1; i < args->queue->size; i++) {
            if (array[i]->burst < min) {
                min = array[i]->burst;
                task_index = i;
            }
        }

        args->running_task = tasklist_pop_at(args->queue, task_index);
        return true;
    }

    return false;
}

bool psjf_scheduler(SchedulerArgs *args, long cpu_tick) {
    if (!args) return false;

    if (tasklist_empty(args->queue)) return false;

    Task **array = args->queue->array;
    size_t task_index = 0;
    long min;
    bool flag = false;

    if (!args->running_task) {
        min = array[0]->remaining_burst;
    } else {
        min = args->running_task->remaining_burst;
    }

    for (size_t i = 0; i < args->queue->size; i++) {
        if (array[i]->remaining_burst < min) {
            min = array[i]->remaining_burst;
            task_index = i;
            flag = true;
        }
    }

    if (!args->running_task) {
        // There is no running task.
        args->running_task = tasklist_pop_at(args->queue, task_index);
        return true;
    } else if (!flag) {
        // There is a running task and it is the shortest job.
        return false;
    } else {
        // There is a running task and it is not the shortest job.
        Task *task = tasklist_pop_at(args->queue, task_index);
        tasklist_push(args->queue, args->running_task, false);
        args->running_task = task;
        return true;
    }
}
