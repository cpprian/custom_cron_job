#include "server_task.h"

void process_message(mqd_t server_queue, char* buffer) {
    char* command = strtok(buffer, " ");
    struct arg_struct* arg_struct = NULL;
    if (strcmp(command, "listte") == 0) {
        char userid[1024];
        if (mq_receive(server_queue, userid, 1024, NULL) == -1) {
            rtlsp_logl(MESSAGE_WARNING, MEDIUM, "Failed to receive message from client");
            return;
        }
        cron_list_all(userid);
    } else if (strcmp(command, "remove") == 0) {
        char* id = strtok(NULL, " ");
        char* args[1] = {id};
        arg_struct = arg_struct_init(ARG_STRUCT_REMOVE, args);
        cron_remove(arg_struct->ID);
    } else if (strcmp(command, "create") == 0) {
        char* abs = strtok(NULL, " ");
        char* msec = strtok(NULL, " ");
        char* sec = strtok(NULL, " ");
        char* rmsec = strtok(NULL, " ");
        char* rsec = strtok(NULL, " ");
        char* cmd = strtok(NULL, " ");
        char* args[6] = {abs, msec, sec, rmsec, rsec, cmd};
        arg_struct = arg_struct_init(ARG_STRUCT_CREATE, args);
        cron_add(arg_struct);
    } else {
        printf("Unknown command\n");
    }
}