#include "cron_timer.h"

time_t return_tv_sec(struct arg_struct* arg) {
    time_t tv_sec = arg->start_sec;

    if (arg->abs == 1) {
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        tv_sec += current_time.tv_sec;
    }

    printf("tv_sec: %zu\n", tv_sec);
    return tv_sec;
}

long return_tv_nsec(struct arg_struct* arg) {
    long tv_nsec = arg->start_msec;

    if (arg->abs) {
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        tv_nsec += current_time.tv_sec;
    }

    return tv_nsec;
}

time_t return_interval_sec(struct arg_struct* arg) {
    time_t interval_sec = arg->repeat_sec;
    return interval_sec;
}

long return_interval_nsec(struct arg_struct* arg) {
    return arg->repeat_msec;
}