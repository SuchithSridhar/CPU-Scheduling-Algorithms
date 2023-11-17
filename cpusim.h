#ifndef CPUSIM_H
#define CPUSIM_H

#include <stdbool.h>
#include "task_management.h"

typedef bool (*SchedulerFunction) (Task **running, TaskList *queue);

bool task_schedule(SchedulerFunction scheduler, Task **runnning, TaskList *queue);

bool fcfs_scheduler(Task **running, TaskList *queue);
 
#endif
