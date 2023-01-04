#include "./rtlsp/rtlsp.h"
#include "./rtlsp/message.h"
#include "./config/config.h"
#include "./util/arg_handler.h"
#include "./cron_util/cron_util.h"

int main(int argc, char* argv[]) {
    sem_cron = *sem_open("sem_cron", O_CREAT | O_RDWR, 0644, 1);
    rtlsp_init(STANDARD, DEFAULT_LOG_PATH, DEFAULT_DUMP_PATH, SIGRTMIN, SIGRTMIN+1);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        rtlsp_logl(MESSAGE_ERROR, HIGH, "Failed to open shared memory");
        return 1;
    }

    int pid_file = open(PID_FILE, O_CREAT | O_RDWR, 0644);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if (rc) {
        // client
        if (EWOULDBLOCK == errno) {
            rtlsp_logl(MESSAGE_INFO, LOW, "New client has been started");
            struct arg_struct* arg = arg_struct_init(argc, argv);
            if (arg == NULL) {
                rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to initialize arg_struct");
                sem_close(&sem_cron);
                rtlsp_destroy();
                return 1;
            }

            if (arg->request == REQUEST_LIST) {
                rtlsp_logl(MESSAGE_INFO, LOW, "Requesting list of cron jobs");
            } else if (arg->request == REQUEST_REMOVE) {
                rtlsp_loglf(MESSAGE_INFO, LOW, "Requesting removal of cron job with ID %zu", arg->ID);
            } else if (arg->request == REQUEST_ADD) {
                rtlsp_loglf(MESSAGE_INFO, LOW, "Requesting addition of cron job with command %s", arg->command);
            } else {
                rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Invalid request type");
                sem_close(&sem_cron);
                rtlsp_destroy();
                return 1;
            }

            save_arg_struct(arg, shm_fd);

            union sigval value;
            value.sival_int = shm_fd;
            if (sigqueue(getppid(), SIGRTMIN+2, value) == -1) {
                rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to send signal to parent");
                sem_close(&sem_cron);
                rtlsp_destroy();
                return 1;
            }
            rtlsp_logl(MESSAGE_INFO, LOW, "Signal sent to parent");
        } else {
            rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to lock PID file. Cannot start client...");
            sem_close(&sem_cron);
            return 1;
        }
    } else {
        // server
        rtlsp_logl(MESSAGE_INFO, LOW, "New server has been started");
        cron_init();

        while(1) {
            // wait for signal
            sigset_t mask;
            sigemptyset(&mask);
            sigaddset(&mask, SIGRTMIN+2);
            sigaddset(&mask, SIGINT);

            siginfo_t info;
            sigwaitinfo(&mask, &info);

            if (info.si_signo == SIGINT) {
                rtlsp_logl(MESSAGE_INFO, LOW, "SIGINT received. Exiting...");
                break;
            }

            if (info.si_signo == SIGRTMIN+2) {
                rtlsp_logl(MESSAGE_INFO, LOW, "Signal received");
                struct arg_struct* arg = load_arg_struct(shm_fd);
                if (arg == NULL) {
                    rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Failed to load arg_struct");
                    sem_close(&sem_cron);
                    rtlsp_destroy();
                    return 1;
                }

                if (arg->request == REQUEST_LIST) {
                    rtlsp_logl(MESSAGE_INFO, LOW, "Requesting list of cron jobs");
                    cron_list_all();
                } else if (arg->request == REQUEST_REMOVE) {
                    rtlsp_loglf(MESSAGE_INFO, LOW, "Requesting removal of cron job with ID %zu", arg->ID);
                    cron_remove(arg->ID);
                } else if (arg->request == REQUEST_ADD) {
                    rtlsp_loglf(MESSAGE_INFO, LOW, "Requesting addition of cron job with command %s", arg->command);
                    cron_add(arg);
                } else {
                    rtlsp_logl(MESSAGE_CRITICAL, HIGH, "Invalid request type");
                    sem_close(&sem_cron);
                    rtlsp_destroy();
                    return 1;
                }
            }
        }

        cron_destroy();
        sem_unlink("sem_cron");
    }

    rtlsp_destroy();
    return 0;
}