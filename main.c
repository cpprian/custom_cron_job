#include "./rtlsp/rtlsp.h"
#include "./rtlsp/message.h"
#include "./config/config.h"

int main(int argc, char* argv[]) {
    rtlsp_init(MIN, DEFAULT_LOG_PATH, DEFAULT_DUMP_PATH, SIGRTMIN, SIGRTMIN + 1);
    rtlsp_loglf(MESSAGE_INFO, LOW, "Hello, %s!", "World");
    rtlsp_sig(getpid(), SIGRTMIN + 1, 1);
    rtlsp_destroy();

    // TODO: check is server running
    // TODO: server.run -> server_init -> server_destroy
    // TODO: client.run -> client_init -> client_destroy
    return 0;
}