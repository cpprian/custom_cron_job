#include "arg_handler.h"

struct arg_struct* arg_struct_init(int argc, char* argv[]) {
    struct arg_struct* arg_struct = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
    if (arg_struct == NULL) {
        rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to allocate memory for arg_struct");
        return NULL;
    }
    char* endptr;
    arg_struct->ID = -1;

    if (argc == ARG_STRUCT_REMOVE) {
        arg_struct->request = REQUEST_REMOVE;
        arg_struct->ID = strtol(argv[0], &endptr, 10);
        if (endptr == argv[0]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse ID");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
    } else if (argc == ARG_STRUCT_CREATE) {
        arg_struct->request = REQUEST_ADD;

        arg_struct->abs = (int)strtol(argv[0], &endptr, 10);
        if (endptr == argv[0]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse abs");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
        arg_struct->repeat_msec = strtol(argv[1], &endptr, 10);
        if (endptr == argv[1]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse repeat_msec");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
        arg_struct->repeat_sec = strtol(argv[2], &endptr, 10);
        if (endptr == argv[3]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse repeat_sec");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
        arg_struct->start_sec = strtol(argv[3], &endptr, 10);
        if (endptr == argv[3]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse start_sec");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
        arg_struct->start_msec = strtol(argv[4], &endptr, 10);
        if (endptr == argv[4]) {
            rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Failed to parse start_msec");
            arg_struct_destroy(arg_struct);
            return NULL;
        }
        arg_struct->command = argv[5];
    } else {
        rtlsp_logl( MESSAGE_ERROR, MEDIUM, "arg_struct_init: Wrong number of arguments");
        arg_struct_destroy(arg_struct);
        return NULL;
    }

    return arg_struct;
}

void arg_struct_destroy(struct arg_struct* arg_struct) {
    free(arg_struct);
}