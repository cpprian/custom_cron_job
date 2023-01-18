#include "client_task.h"

// client arguments for creating new job: abs msec sec rmsec rsec cmd
// client arguments for removing job: "remove" id
// client arguments for listing jobs: "list"
void handle_client(mqd_t client_queue, int argc, char* argv[]) {
    printf("Client connected, args: %d\n", argc);
    switch(argc) {
        case 3: {
            if (strcmp(argv[1], "list") == 0) {
                list_tasks(client_queue, argv[2]);
            } else if (strcmp(argv[1], "remove") == 0) {
                remove_task(client_queue, argv[2]);
            } else {
                printf("Unknown command\n");
            }
            break;
        }
        case 7: {
            create_task(client_queue, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
            break;
        }
        default:
            printf("Unknown command\n");
            break;
    }
}

void list_tasks(mqd_t client_queue, char* userid) {
    char buffer[1024];
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;
    mqd_t list_queue = mq_open(userid,  O_CREAT | O_RDONLY, 0666, &attr);
    if (list_queue == -1) {
        printf("Failed to open list queue\n");
        return;
    }

    strcpy(buffer, "listte");
    if (mq_send(client_queue, buffer, strlen(buffer), 0) == -1) {
        printf("Failed to send message to server\n");
        return;
    }
    if (mq_send(client_queue, userid, strlen(userid), 0) == -1) {
        printf("Failed to send message to server\n");
        return;
    }

    while (1) {
        printf("Czekam\n");
        if (mq_receive(list_queue, buffer, 1024, NULL) == -1) {
            printf("Failed to receive message from server\n");
            return;
        }
        if (strcmp(buffer, "END") == 0) {
            return;
        }
        printf("%s\n", buffer);
    }
}

void remove_task(mqd_t client_queue, char* id) {
    char buffer[1024];
    strcpy(buffer, "remove ");
    strcat(buffer, id);
    if (mq_send(client_queue, buffer, strlen(buffer), 0) == -1) {
        printf("Failed to send message to server\n");
    }
}

void create_task(mqd_t client_queue, char* abs, char* msec, char* sec, char* rmsec, char* rsec, char* cmd) {
    char* buffer = (char*)calloc(1024, sizeof(char));

    strcpy(buffer, "create ");
    strcat(buffer, abs);
    strcat(buffer, " ");
    strcat(buffer, msec);
    strcat(buffer, " ");
    strcat(buffer, sec);
    strcat(buffer, " ");
    strcat(buffer, rmsec);
    strcat(buffer, " ");
    strcat(buffer, rsec);
    strcat(buffer, " ");
    strcat(buffer, cmd);
    strcat(buffer, " ");

    if (mq_send(client_queue, buffer, strlen(buffer), 0) == -1) {
        printf("Failed to send message to server\n");
    }
    free(buffer);
}