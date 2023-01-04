#include "arg_handler.h"

struct arg_struct* arg_struct_init(int argc, char* argv[]) {
    if (argc == ARG_STRUCT_LIST_ARGS && strcmp(argv[1], "-l") == 0) {
        struct arg_struct* arg = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
        if (arg == NULL) {
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for arg");
            return NULL;
        }
        arg->request = REQUEST_LIST;
        arg->ID = -1;
        return arg;
    } else if (argc == ARG_STRUCT_REMOVE_ARGS && strcmp(argv[1], "-c") == 0) {
        struct arg_struct* arg = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
        if (arg == NULL) {
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to allocate memory for arg");
            return NULL;
        }
        arg->request = REQUEST_REMOVE;

        char* endptr;
        arg->ID = strtol(argv[2], &endptr, 10);
        if (endptr == argv[2]) {
            arg_struct_destroy(arg);
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to convert ID to number");
            return NULL;
        }
        return arg;
    }
    if (argc != ARG_STRUCT_MAX_ARGS) {
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid number of arguments: %d", argc);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Usage: %s <minute> <hour> <day> <month> <weekday> <command> [output] [log]\n", argv[0]);
        return NULL;
    }

    struct arg_struct* arg_struct = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
    if (arg_struct == NULL) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    arg_struct->request = REQUEST_ADD;
    arg_struct->ID = -1;
    arg_struct->repeat = 0;

    char* err;
    arg_struct->abs = strtol(argv[1], &err, 10);
    if (err == argv[1]) {
        arg_struct_destroy(arg_struct);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid argument ABS: %s", argv[1]);
        return NULL;
    } else if (arg_struct->abs < 0 || arg_struct->abs > 1) {
        arg_struct_destroy(arg_struct);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid argument ABS: %s", argv[1]);
        return NULL;
    }

    arg_struct->repeat = strtol(argv[2], &err, 10);
    if (err == argv[2]) {
        arg_struct_destroy(arg_struct);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid argument REPEAT: %s", argv[2]);
        return NULL;
    } else if (arg_struct->repeat < 0 || arg_struct->repeat > 1) {
        arg_struct_destroy(arg_struct);
        rtlsp_loglf(MESSAGE_ERROR, HIGH, "Invalid argument REPEAT: %s", argv[2]);
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

    strcpy(arg_struct->schedule->minute, argv[3]);
    strcpy(arg_struct->schedule->hour, argv[4]);
    strcpy(arg_struct->schedule->day, argv[5]);
    strcpy(arg_struct->schedule->month, argv[6]);
    strcpy(arg_struct->schedule->weekday, argv[7]);
    strcpy(arg_struct->command, argv[8]);
    strcpy(arg_struct->output, argv[9]);

    return arg_struct;
}

void save_arg_struct(struct arg_struct* arg_struct, int shm_fd) {
    if (ftruncate(shm_fd, sizeof(struct arg_struct)) == -1) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to truncate shared memory");
        return;
    }

    void* shm_ptr = mmap(NULL, sizeof(struct arg_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to map shared memory");
        return;
    }

    memcpy(shm_ptr, arg_struct, sizeof(struct arg_struct));

    if (munmap(shm_ptr, sizeof(struct arg_struct)) == -1) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to unmap shared memory");
        return;
    }
    shm_unlink(SHM_NAME);
}

struct arg_struct* load_arg_struct(int shm_fd) {
    struct arg_struct* arg_struct = (struct arg_struct*)calloc(1, sizeof(struct arg_struct));
    if (arg_struct == NULL) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, ERR_ALLOC);
        return NULL;
    }

    void* shm_ptr = mmap(NULL, sizeof(struct arg_struct), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to map shared memory");
        return NULL;
    }

    memcpy(arg_struct, shm_ptr, sizeof(struct arg_struct));

    if (munmap(shm_ptr, sizeof(struct arg_struct)) == -1) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to unmap shared memory");
        return NULL;
    }
    shm_unlink(SHM_NAME);

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