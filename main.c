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

    /* TaskList *queue = tasklist_create(); */

    /* long cpu_clock = 0; */

    /* while (true) { */
    /*     // Checks to see if task has to be added to queue and adds it. */
    /*     task_process_arrival(tasklist, queue, cpu_clock); */

    /*     cpu_clock++; */
    /* } */

    tasklist_print(tasklist);
    tasklist_destory(tasklist);

    return EXIT_SUCCESS;
}
