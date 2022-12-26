#ifndef CRON_UTIL_H
#define CRON_UTIL_H

#include "../config/config.h"
#include "../rtlsp/rtlsp.h"
#include "../rtlsp/message.h"

struct cron_struct {
    size_t ID;
    char *command;
    char *time;
};

struct ll_cron {
    struct cron_struct *cron;
    struct ll_cron *next;
};

struct ll_cron *cron_list;

void cron_init();
void cron_add(char *command, char *time);
void cron_remove(size_t ID);
void cron_list_all();
void cron_run(struct ll_cron *cron);
void cron_destroy();
void cron_destroy_cron(struct ll_cron *cron);

#endif // CRON_UTIL_H