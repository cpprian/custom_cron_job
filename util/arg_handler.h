#ifndef ARG_HANDLER_H
#define ARG_HANDLER_H

#include "../config/config.h"
#include "../rtlsp/rtlsp.h"
#include "../rtlsp/message.h"

#define ARG_STRUCT_REMOVE       1
#define ARG_STRUCT_CREATE       6

enum request_type {
    REQUEST_LIST,
    REQUEST_ADD,
    REQUEST_REMOVE
};

struct arg_struct {
    enum request_type request;
    size_t ID; // -1 or number of cron job

    int abs;
    long repeat_msec;
    long repeat_sec;
    long start_sec;
    long start_msec;
    char* command;
};

struct arg_struct* arg_struct_init(int argc, char* argv[]);
void arg_struct_destroy(struct arg_struct* arg_struct);

#endif // ARG_HANDLER_H