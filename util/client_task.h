#ifndef CLIENT_TASK_H
#define CLIENT_TASK_H

#include "../rtlsp/rtlsp.h"
#include "../cron_util/cron_util.h"
#include "arg_handler.h"

void handle_client(mqd_t client_queue, int argc, char* argv[]);
void list_tasks(mqd_t client_queue, char* userid);
void remove_task(mqd_t client_queue, char* id);
void create_task(mqd_t client_queue, char* abs, char* msec, char* sec, char* rmsec, char* rsec, char* cmd);

#endif //CLIENT_TASK_H
