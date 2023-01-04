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

void cron_add(struct arg_struct* arg) {
    sem_wait(&sem_cron);
    if (arg->request == REQUEST_LIST) {
        cron_list_all();
        arg_struct_destroy(arg);
        return;
    } else if (arg->request == REQUEST_REMOVE) {
        cron_remove(arg->ID);
        arg_struct_destroy(arg);
        return;
    } else if (arg->request != REQUEST_ADD) {
        rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Invalid request type");
        return;
    }

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

    new_cron->cron->arg = arg;

    if (last_cron != NULL) {
        last_cron->next = new_cron;
    } else {
        cron_list = new_cron;
    }

    cron_run(new_cron);
    sem_wait(&sem_cron);
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

void cron_list_all() {
    struct ll_cron *current = cron_list;
    while (current != NULL) {
        printf("ID: %zu\n", current->cron->ID);
        printf("\tCommand: %s\n", current->cron->arg->command);
        printf("\tOutput: %s\n", current->cron->arg->output);
        printf("\tTime: hour: %s, minute: %s, day: %s, month: %s, weekday: %s\n", current->cron->arg->schedule->hour, current->cron->arg->schedule->minute, current->cron->arg->schedule->day, current->cron->arg->schedule->month, current->cron->arg->schedule->weekday);
        current = current->next;
    }
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

void cron_run(struct ll_cron *cron) {
    timer_t timerid;
    // fixme: check is still correct
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
}

void* cron_timeout(void* arg) {
    sem_wait(&sem_cron);
    struct ll_cron *cron = (struct ll_cron*)arg;
    if (cron == NULL) {
        rtlsp_loglf(MESSAGE_WARNING, HIGH, "cron_timeout: cron_struct is NULL");
        return NULL;
    }

    pid_t child_pid;
    char* argv[] = {cron->cron->arg->command, cron->cron->arg->output, NULL};
    posix_spawn(&child_pid, cron->cron->arg->command, NULL, NULL, argv, NULL);

    int status;
    do {
        waitpid(child_pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    rtlsp_loglf(MESSAGE_INFO, LOW, "Cron job with ID %zu finished\n", cron->cron->ID);

    if (!cron->cron->arg->repeat) {
        cron_remove(cron->cron->ID);
    }
    sem_wait(&sem_cron);
    return NULL;
}