#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include "../config/config.h"
#include "../rtlsp/rtlsp.h"
#include "../rtlsp/message.h"

#define ARG_STRUCT_LIST_ARGS    2
#define ARG_STRUCT_REMOVE_ARGS  3
#define ARG_STRUCT_MAX_ARGS     9
#define MAX_COMMAND_SIZE        20
#define MAX_SCHEDULE_SIZE       20
#define MAX_OUTPUT_SIZE         20

enum request_type {
    REQUEST_LIST,
    REQUEST_ADD,
    REQUEST_REMOVE
};

struct schedule {
    char* minute;
    char* hour;
    char* day;
    char* month;
    char* weekday;
};

struct arg_struct {
    enum request_type request;
    size_t ID; // -1 or number of cron job

    int abs;
    int repeat;

    struct schedule* schedule;
    char* command;
    char* output;
};

struct arg_struct* arg_struct_init(int argc, char* argv[]);
void arg_struct_destroy(struct arg_struct* arg_struct);

#endif // ARG_HANDLER_H