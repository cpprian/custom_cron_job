#include "cron_util.h"

struct ll_cron* return_last_cron() {
    struct ll_cron *current = cron_list;
    while (current->next != NULL) {
        current = current->next;
    }
    return current;
}

void cron_init() {
    cron_list = (struct ll_cron*)calloc(1, sizeof(struct ll_cron));
    if (cron_list == NULL) {
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for cron_list");
    }
}

void cron_add(char *command, char *time) {
    struct ll_cron *new_cron = (struct ll_cron*)calloc(1, sizeof(struct ll_cron));
    if (new_cron == NULL) {
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for new_cron");
        return;
    }

    new_cron->cron = (struct cron_struct*)calloc(1, sizeof(struct cron_struct));
    if (new_cron->cron == NULL) {
        cron_destroy_cron(new_cron);
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for new_cron->cron");
        return;
    }

    struct ll_cron* last_cron = return_last_cron();
    new_cron->cron->ID = 1 + last_cron != NULL ? last_cron->cron->ID : 0;

    new_cron->cron->command = (char*)calloc(strlen(command) + 1, sizeof(char));
    if (new_cron->cron->command == NULL) {
        cron_destroy_cron(new_cron);
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for new_cron->cron->command");
        return;
    }
    strcpy(new_cron->cron->command, command);

    new_cron->cron->time = (char*)calloc(strlen(time) + 1, sizeof(char));
    if (new_cron->cron->time == NULL) {
        cron_destroy_cron(new_cron);
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for new_cron->cron->time");
        return;
    }
    strcpy(new_cron->cron->time, time);

    if (last_cron != NULL) {
        last_cron->next = new_cron;
    } else {
        cron_list = new_cron;
    }

    cron_run(new_cron);
}

// Removing occurs when the user executes "make run ID='ID'" or times out for a task.
void cron_remove(size_t ID) {
    struct ll_cron *current = cron_list;
    struct ll_cron *previous = NULL;
    while (current != NULL) {
        if (current->cron->ID == ID) {
            if (previous == NULL) {
                cron_list = current->next;
            } else {
                previous->next = current->next;
            }
            cron_destroy_cron(current);
            return;
        } else if (current->cron->ID > ID) {
            rtlsp_loglf(MESSAGE_WARNING, MEDIUM, "Failed to find cron job with ID %zu\n", ID);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void cron_list_all() {
    struct ll_cron *current = cron_list;
    while (current != NULL) {
        printf("ID: %zu Command: %s Time: %s\n", current->cron->ID, current->cron->command, current->cron->time);
        current = current->next;
    }
}

void cron_destroy_cron(struct ll_cron *cron) {
    free(cron->cron->command);
    free(cron->cron->time);
    free(cron->cron);
    free(cron);
}

void cron_destroy() {
    struct ll_cron *current = cron_list;
    struct ll_cron *next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current->cron->command);
        free(current->cron->time);
        free(current->cron);
        free(current);
        current = next;
    }

    free(cron_list);
}

void cron_run(struct ll_cron *cron) {
    // timers
    // posix spawn
    // waitpid
}