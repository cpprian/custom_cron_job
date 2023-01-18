#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <spawn.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <errno.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <mqueue.h>

#define DEFAULT_LOG_PATH "./out/log"
#define DEFAULT_DUMP_PATH "./out/dump"
#define QUEUE_NAME "/server_queue"

#endif // CONFIG_H