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

#define DEFAULT_LOG_PATH "./out/log"
#define DEFAULT_DUMP_PATH "./out/dump"
#define PID_FILE "./out/custom_cron_job.pid"
#define SHM_NAME "/custom_cron_job_shm"

#endif // CONFIG_H