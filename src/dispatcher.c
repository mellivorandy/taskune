#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "task.h"
#include "common.h"

#define RANGE_START 1
#define RANGE_END 200000000

typedef struct {
    char ip[64];
    int port;
    int start;
    int end;
    int result;
} TaskContext;

void* worker_thread(void* arg) {
    TaskContext* ctx = (TaskContext*)arg;

    cJSON* task = cJSON_CreateObject();
    cJSON_AddStringToObject(task, "type", "prime");
    cJSON_AddNumberToObject(task, "start", ctx->start);
    cJSON_AddNumberToObject(task, "end", ctx->end);
    char* task_str = cJSON_PrintUnformatted(task);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in peer_addr;
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(ctx->port);
    inet_pton(AF_INET, ctx->ip, &peer_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) == 0) {
        send(sock, task_str, strlen(task_str), 0);
        printf("[Master] Send tasks %d~%d to %s:%d\n", ctx->start, ctx->end, ctx->ip, ctx->port);

        char buffer[128] = {0};
        read(sock, buffer, sizeof(buffer));
        cJSON* result = cJSON_Parse(buffer);
        ctx->result = cJSON_GetObjectItem(result, "result")->valueint;
        cJSON_Delete(result);
    } else {
        printf("Cannot connect to %s:%d\n", ctx->ip, ctx->port);
        ctx->result = 0;
    }

    close(sock);
    cJSON_Delete(task);
    free(task_str);
    return NULL;
}

void* self_compute(void* arg) {
    TaskContext* ctx = (TaskContext*)arg;
    int count = 0;
    for (int i = ctx->start; i <= ctx->end; i++) {
        if (is_prime(i)) count++;
    }
    ctx->result = count;
    printf("[Master-MA] Compution finished: there are %d primes betweeen %d~%d\n", count, ctx->start, ctx->end);
    return NULL;
}

void* dispatch_thread(void* arg) {
    char** peers = (char**)arg;

    int num_workers = 0;
    while (peers[num_workers] != NULL) num_workers++;

    int total_parts = num_workers + 1;
    int range_per_part = (RANGE_END - RANGE_START + 1) / total_parts;

    TaskContext contexts[total_parts];
    pthread_t threads[total_parts];

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    contexts[0].start = RANGE_START;
    contexts[0].end = contexts[0].start + range_per_part - 1;
    pthread_create(&threads[0], NULL, self_compute, &contexts[0]);

    for (int i = 0; i < num_workers; i++) {
        sscanf(peers[i], "%[^:]:%d", contexts[i + 1].ip, &contexts[i + 1].port);

        contexts[i + 1].start = RANGE_START + (i + 1) * range_per_part;
        contexts[i + 1].end = (i == num_workers - 1)
                            ? RANGE_END
                            : contexts[i + 1].start + range_per_part - 1;

        pthread_create(&threads[i + 1], NULL, worker_thread, &contexts[i + 1]);
    }

    int total = 0;
    for (int i = 0; i < total_parts; i++) {
        pthread_join(threads[i], NULL);
        total += contexts[i].result;
    }

    gettimeofday(&end, NULL);
    double time = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1e6;

    printf("Tasks completed, found %d primes, taking %.3f s\n", total, time);
    return NULL;
}
