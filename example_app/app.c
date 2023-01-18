#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <spawn.h>
#include <pthread.h>
#include <semaphore.h>

#include "../rtlsp/rtlsp.h"
#include "../rtlsp/message.h"

sem_t sem;
int count = 0;

void* app(void* arg) {
    while(count < 100) {
        sem_wait(&sem);
        count++;
        rtlsp_loglf(MESSAGE_INFO, MEDIUM, "Count: %d (Thread: %d)", count, (int)pthread_self());
        sem_post(&sem);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    printf("Hello world\n");
//    rtlsp_init(STANDARD, ".", ".", SIGRTMIN+3, SIGRTMIN+4);
//    sem_init(&sem, 0, 1);
//
//    pthread_t thread;
//    pthread_t thread2;
//    pthread_create(&thread, NULL, app, NULL);
//    pthread_create(&thread2, NULL, app, NULL);
//    pthread_join(thread, NULL);
//    pthread_join(thread2, NULL);
//
//    rtlsp_destroy();
//    sem_destroy(&sem);
    return 0;
}
