#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include "../config/config.h"
#include "../rtlsp/rtlsp.h"
#include "../rtlsp/message.h"

#define ARG_STRUCT_MIN_ARGS     7
#define ARG_STRUCT_MAX_ARGS     8
#define MAX_SCHEDULE_SIZE       20
#define MAX_COMMAND_SIZE        20
#define MAX_OUTPUT_SIZE         20

struct schedule {
    char* minute;
    char* hour;
    char* day;
    char* month;
    char* weekday;
};

struct arg_struct {
    struct schedule* schedule;
    char* command;
    char* output;
};

struct arg_struct* arg_struct_init(int argc, char* argv[]);
void arg_struct_destroy(struct arg_struct* arg_struct);

#endif // ARG_HANDLER_H