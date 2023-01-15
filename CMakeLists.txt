cmake_minimum_required(VERSION 3.24)
project(custom_cron_job C)

set(CMAKE_C_STANDARD 11)

include_directories(rtlsp)
include_directories(config)

add_executable(custom_cron_job main.c
        rtlsp/message.h
        rtlsp/rtlsp.c
        rtlsp/rtlsp.h

        cron_util/cron_util.h
        cron_util/cron_util.c

        cron_util/cron_timer/cron_timer.c
        cron_util/cron_timer/cron_timer.h

        config/config.h
        util/arg_handler.c
        util/arg_handler.h example_app/app.c)
