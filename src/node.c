#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common.h"

void* server_thread(void* arg);
void* dispatch_thread(void* arg);

int main(int argc, char* argv[]) {
    if ((strcmp(argv[1], "master") == 0 && argc < 4) ||
        (strcmp(argv[1], "worker") == 0 && argc < 3)) {
        printf("Usage: %s <mode: master|worker> <self_port> <peer_ip:port> ...\n", argv[0]);
        return 1;
    }

    const char* mode = argv[1];
    int self_port = atoi(argv[2]);

    pthread_t srv_tid, dispatch_tid;

    // Start TCP server (receive tasks)
    pthread_create(&srv_tid, NULL, server_thread, &self_port);

    // If it is a master, then issue jobs
    if (strcmp(mode, "master") == 0) {
        pthread_create(&dispatch_tid, NULL, dispatch_thread, &argv[3]);
        pthread_join(dispatch_tid, NULL);
    }

    pthread_join(srv_tid, NULL);
    return 0;
}
