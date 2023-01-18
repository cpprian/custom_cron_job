#ifndef SERVER_TASK_H
#define TASK_H

#include "../rtlsp/rtlsp.h"
#include "../cron_util/cron_util.h"
#include "arg_handler.h"

void process_message(mqd_t server_queue, char* buffer);

#endif //SERVER_TASK_H
