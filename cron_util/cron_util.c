#include "cron_util.h"

struct ll_cron *cron_list;

struct ll_cron* return_last_cron() {
    struct ll_cron *current = cron_list;
    if (current == NULL) {
        return NULL;
    }
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

void cron_add(struct arg_struct* arg) {
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
    new_cron->cron->ID = 1 + (last_cron != NULL ? last_cron->cron->ID : 0);

    new_cron->cron->arg = arg;

    if (last_cron != NULL) {
        last_cron->next = new_cron;
    } else {
        cron_list = new_cron;
    }

    cron_run(new_cron);
}

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
            rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Removed cron job with ID %zu\n", ID);
            return;
        } else if (current->cron->ID > ID) {
            rtlsp_loglf(MESSAGE_WARNING, MEDIUM, "Failed to find cron job with ID %zu\n", ID);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void cron_list_all(char* user_id) {
    struct ll_cron *current = cron_list;
    mqd_t client_queue = mq_open(user_id, O_WRONLY);
    if (client_queue == -1) {
        rtlsp_loglf(MESSAGE_WARNING, MEDIUM, "Failed to open client queue %s\n", client_queue);
        return;
    }

    while (current != NULL) {
        char* message = (char*)calloc(1, 100);
        if (message == NULL) {
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for message");
            return;
        }
        sprintf(message, "%zu:%s", current->cron->ID, current->cron->arg->command);
        if (mq_send(client_queue, message, strlen(current->cron->arg->command), current->cron->ID) == -1) {
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to send message to client");
            free(message);
            return;
        }
        free(message);
        current = current->next;
    }
    if (mq_send(client_queue, "END", 3, 0) == -1) {
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to send message to client");
    }
    mq_close(client_queue);
    rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Listed all cron jobs");
}

void cron_destroy_cron(struct ll_cron *cron) {
    arg_struct_destroy(cron->cron->arg);
    timer_delete(&(cron->cron->timer));
    free(cron->cron);
    free(cron);
}

void cron_destroy() {
    struct ll_cron *current = cron_list;
    struct ll_cron *next = NULL;
    while (current != NULL) {
        next = current->next;
        cron_destroy_cron(current);
        current = next;
    }

    free(cron_list);
}

void* cron_timeout(void* arg) {
    struct ll_cron *cron = (struct ll_cron*)arg;
    if (cron == NULL) {
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to get cron from sigval");
        return NULL;
    }

    pid_t child_pid;
    char* argv[] = {cron->cron->arg->command, NULL};
    posix_spawn(&child_pid, cron->cron->arg->command, NULL, NULL, argv, NULL);

    int status;
    do {
        waitpid(child_pid, &status, 0);
    } while (!WIFEXITED(status));

    if (cron->cron->arg->repeat_msec == 0 && cron->cron->arg->repeat_sec == 0) {
        cron_remove(cron->cron->ID);
        rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Cron job with ID %zu timed out", cron->cron->ID);
    } else {
        rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Cron job with ID %zu will be repeated", cron->cron->ID);
    }
    return NULL;
}

void cron_run(struct ll_cron *cron) {
    timer_t timerid;

    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = cron_timeout;
    sev.sigev_value.sival_ptr = (void*)cron;
    sev.sigev_notify_attributes = NULL;
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    struct itimerspec timer;
    timer.it_value.tv_sec = return_tv_sec(cron->cron->arg);
    timer.it_value.tv_nsec = return_tv_nsec(cron->cron->arg);
    timer.it_interval.tv_sec = return_interval_sec(cron->cron->arg);
    timer.it_interval.tv_nsec = return_interval_nsec(cron->cron->arg);

    if (cron->cron->arg->abs) {
        timer_settime(timerid, TIMER_ABSTIME, &timer, NULL);
    } else {
        timer_settime(timerid, 0, &timer, NULL);
    }

    cron->cron->timer = timer;
    rtlsp_logl(MESSAGE_INFO, MEDIUM, "Started cron job");
}