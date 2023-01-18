#include "./rtlsp/rtlsp.h"
#include "./util/arg_handler.h"
#include "./util/client_task.h"
#include "./util/server_task.h"


int check_server() {
    mqd_t mqd = mq_open(QUEUE_NAME, O_RDONLY);
    if (mqd == -1) {
        return 0;
    }
    mq_close(mqd);
     mq_unlink(QUEUE_NAME);
    return 1;
}

volatile sig_atomic_t server_running = 1;

void close_server_handler(int signum, siginfo_t* info, void* other) {
    server_running = 0;
}

int main(int argc, char* argv[]) {
    pid_t child_pid;
    mqd_t server_queue, client_queue;
    char buffer[1024];
    struct mq_attr attr;
    rtlsp_init(MIN, DEFAULT_LOG_PATH, DEFAULT_DUMP_PATH, SIGRTMIN, SIGRTMIN+1);

    // Sprawdzanie, czy serwer jest uruchomiony
    if (check_server()) {
        // Serwer jest uruchomiony, ale klient chce coś zrobić
        // Wysyłanie żądania do serwera
        client_queue = mq_open(QUEUE_NAME, O_WRONLY);
        if (client_queue == -1) {
            printf("Failed to open client queue\n");
            return 1;
        }

        handle_client(client_queue, argc, argv);

        mq_close(client_queue);
        rtlsp_destroy();
        return 0;
    }

    // Tworzenie kolejki serwera
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;
    server_queue = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_queue == -1) {
        printf("Failed to create server queue\n");
        rtlsp_destroy();
        return 1;
    }

    struct sigaction action;
    action.sa_sigaction = close_server_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) == -1) {
        printf("Failed to set signal handler\n");
        rtlsp_destroy();
        return 1;
    }

    // Forkowanie i uruchomienie procesów
    child_pid = fork();
    while(1) {
        if (!server_running) {
            printf("Server is shutting down\n");
            break;
        }
        if (child_pid == 0) {
            // Proces klienta
            client_queue = mq_open(QUEUE_NAME, O_WRONLY);
            if (client_queue == -1) {
                printf("Failed to open client queue\n");
                return 1;
            }

            handle_client(client_queue, argc, argv);

            mq_close(client_queue);
            rtlsp_destroy();
            return 0;
        } else if (child_pid > 0) {
            // Proces serwera
            if (mq_receive(server_queue, buffer, 1024, NULL) == -1) {
                if (server_running) {
                    printf("Failed to receive message from client\n");
                    mq_close(server_queue);
                    mq_unlink(QUEUE_NAME);
                    cron_destroy();
                    rtlsp_destroy();
                    return 1;
                }

                // Serwer został zamknięty sygnałem
                break;
            }
            printf("Received message: %s\n", buffer);
            process_message(server_queue, buffer);
        } else {
            printf("Failed to fork\n");
            mq_close(server_queue);
            mq_unlink(QUEUE_NAME);
            cron_destroy();
            rtlsp_destroy();
            return 1;
        }
    }

    mq_close(server_queue);
    mq_unlink(QUEUE_NAME);
    cron_destroy();
    rtlsp_destroy();
    return 0;
}
