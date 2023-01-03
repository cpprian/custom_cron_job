#include "cron_timer.h"

time_t return_tv_sec(struct arg_struct* arg) {
    time_t tv_sec = 0;

    tv_sec += atoi(arg->schedule->minute) * 60;
    tv_sec += atoi(arg->schedule->hour) * 60 * 60;
    tv_sec += atoi(arg->schedule->day) * 60 * 60 * 24;
    tv_sec += atoi(arg->schedule->month) * 60 * 60 * 24 * 30;
    tv_sec += atoi(arg->schedule->weekday) * 60 * 60 * 24 * 7;

    if (arg->abs == 1) {
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        tv_sec += current_time.tv_sec;
    }

    return tv_sec;
}

long return_tv_nsec(struct arg_struct* arg) {
    long tv_nsec = 0;

    if (arg->abs) {
        struct timespec current_time;
        clock_gettime(CLOCK_REALTIME, &current_time);
        tv_nsec += current_time.tv_sec;
    }

    return tv_nsec;
}

time_t return_interval_sec(struct arg_struct* arg) {
    time_t interval_sec = 0;

    if (arg->repeat && !arg->abs) {
        interval_sec += atoi(arg->schedule->minute) * 60;
    }

    return interval_sec;
}

long return_interval_nsec(struct arg_struct* arg) {
    return 0 ;
}