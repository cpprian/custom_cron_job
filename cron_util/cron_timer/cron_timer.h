#ifndef CRON_TIMER_H
#define CRON_TIMER_H

#include "../../config/config.h"
#include "../../rtlsp/rtlsp.h"
#include "../../rtlsp/message.h"
#include "../../util/arg_handler.h"

time_t return_tv_sec(struct arg_struct* arg);
long return_tv_nsec(struct arg_struct* arg);

time_t return_interval_sec(struct arg_struct* arg);
long return_interval_nsec(struct arg_struct* arg);

#endif // CRON_TIMER_H
