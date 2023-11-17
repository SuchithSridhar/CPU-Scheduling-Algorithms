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

    long clock;

    while (true) {
        clock++;
    }

    return EXIT_SUCCESS;
}
