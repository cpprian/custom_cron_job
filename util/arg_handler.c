#include "arg_handler.h"

struct arg_struct* arg_struct_init(int argc, char* argv[]) {
    if (argc < ARG_STRUCT_MIN_ARGS || argc > ARG_STRUCT_MAX_ARGS) {
        printf("Usage: %s <minute> <hour> <day> <month> <weekday> <command> [output] [log]\n", argv[0]);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid number of arguments: %d", argc);
        return NULL;
    }

    struct arg_struct* arg_struct = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
    if (arg_struct == NULL) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule = (struct schedule*)calloc(1, sizeof(struct schedule));
    if (arg_struct->schedule == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule->minute = (char*)calloc(MAX_SCHEDULE_SIZE, sizeof(char));
    if (arg_struct->schedule->minute == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule->hour = (char*)calloc(MAX_SCHEDULE_SIZE, sizeof(char));
    if (arg_struct->schedule->hour == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule->day = (char*)calloc(MAX_SCHEDULE_SIZE, sizeof(char));
    if (arg_struct->schedule->day == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule->month = (char*)calloc(MAX_SCHEDULE_SIZE, sizeof(char));
    if (arg_struct->schedule->month == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->schedule->weekday = (char*)calloc(MAX_SCHEDULE_SIZE, sizeof(char));
    if (arg_struct->schedule->weekday == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->command = (char*)calloc(MAX_COMMAND_SIZE, sizeof(char));
    if (arg_struct->command == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->output = (char*)calloc(MAX_OUTPUT_SIZE, sizeof(char));
    if (arg_struct->output == NULL) {
        arg_struct_destroy(arg_struct);
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    strcpy(arg_struct->schedule->minute, argv[1]);
    strcpy(arg_struct->schedule->hour, argv[2]);
    strcpy(arg_struct->schedule->day, argv[3]);
    strcpy(arg_struct->schedule->month, argv[4]);
    strcpy(arg_struct->schedule->weekday, argv[5]);
    strcpy(arg_struct->command, argv[6]);
    if (argc == ARG_STRUCT_MAX_ARGS) {
        strcpy(arg_struct->output, argv[7]);
    }

    return arg_struct;
}

void arg_struct_destroy(struct arg_struct* arg_struct) {
    free(arg_struct->schedule->minute);
    free(arg_struct->schedule->hour);
    free(arg_struct->schedule->day);
    free(arg_struct->schedule->month);
    free(arg_struct->schedule->weekday);
    free(arg_struct->command);
    free(arg_struct->output);
    free(arg_struct->schedule);
    free(arg_struct);
}