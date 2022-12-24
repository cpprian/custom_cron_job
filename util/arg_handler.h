#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include "../config/config.h"

#define ARG_HANDLER_MAX_ARGS 8

struct schedule {
    char* minute;
    char* hour;
    char* day;
    char* month;
    char* weekday;
};

struct arg_handler {
    struct schedule schedule;
    char* command;
    char* output;
};

struct arg_handler* arg_handler_init(int argc, char* argv[]);
void arg_handler_destroy(struct arg_handler* arg_handler);

#endif // ARG_HANDLER_H