cmake_minimum_required(VERSION 3.24)
project(custom_cron_job C)

set(CMAKE_C_STANDARD 11)

include_directories(rtlsp)

add_executable(custom_cron_job
        rtlsp/message.h
        rtlsp/rtlsp.c
        rtlsp/rtlsp.h)
