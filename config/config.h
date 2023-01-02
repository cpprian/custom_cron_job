#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>
#include <spawn.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEFAULT_LOG_PATH "./out/log"
#define DEFAULT_DUMP_PATH "./out/dump"

#endif // CONFIG_H