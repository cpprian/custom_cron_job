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
        // todo: print list of all cron jobs
        current = current->next;
    }
    rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Listed all cron jobs");
}

void cron_destroy_cron(struct ll_cron *cron) {
    arg_struct_destroy(cron->cron->arg);
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
    sigset_t mask;
    sigfillset(&mask);

    struct sigaction sa;
    sa.sa_handler = cron_timeout;
    sa.sa_mask = mask;
    sa.sa_flags = (int)cron->cron->ID;

    sigaction(CLOCK_REALTIME, &sa, NULL);

    timer_t timerid;

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = CLOCK_REALTIME;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_REALTIME, &sev, &timerid);

    // TODO: to set the timer to the time specified in the cron job
    struct itimerspec timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_nsec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_nsec = 0;
    timer_settime(timerid, 0, &timer, NULL);
}

void cron_timeout(int signum) {
    struct ll_cron *current = cron_list;
    while (current != NULL) {
        if (current->cron->ID == (size_t)signum) {
            return;
        }
        current = current->next;
    }

    if (current == NULL) {
        rtlsp_loglf(MESSAGE_WARNING, MEDIUM, "Failed to find cron job with ID %d\n", signum);
        return;
    }

    pid_t child_pid;
    char* argv[] = {current->cron->arg->command, current->cron->arg->output, NULL};
    posix_spawn(&child_pid, current->cron->arg->command, NULL, NULL, argv, NULL);

    int status;
    do {
        waitpid(child_pid, &status, 0);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    rtlsp_loglf(MESSAGE_INFO, LOW, "Cron job with ID %zu finished\n", current->cron->ID);

    if (!current->cron->arg->repeat) {
        cron_remove(current->cron->ID);
    }
}