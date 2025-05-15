#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "common.h"

#define RANGE_START 1
#define RANGE_END 200000000

void* dispatch_thread(void* arg) {
    char** peers = (char**)arg;

    int num_workers = 0;
    while (peers[num_workers] != NULL) num_workers++;

    int range_per_worker = (RANGE_END - RANGE_START + 1) / num_workers;

    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    int total_count = 0;

    for (int i = 0; i < num_workers; i++) {
        char ip[64]; int port;
        sscanf(peers[i], "%[^:]:%d", ip, &port);

        int start = RANGE_START + i * range_per_worker;
        int end_range = (i == num_workers - 1) ? RANGE_END : start + range_per_worker - 1;

        cJSON* task = cJSON_CreateObject();
        cJSON_AddStringToObject(task, "type", "prime");
        cJSON_AddNumberToObject(task, "start", start);
        cJSON_AddNumberToObject(task, "end", end_range);
        char* task_str = cJSON_PrintUnformatted(task);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in peer_addr;
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &peer_addr.sin_addr);

        if (connect(sock, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) == 0) {
            send(sock, task_str, strlen(task_str), 0);
            printf("[Master] Send tasks %d~%d to %s:%d\n", start, end_range, ip, port);

            char buffer[128] = {0};
            read(sock, buffer, sizeof(buffer));
            cJSON* result = cJSON_Parse(buffer);
            int count = cJSON_GetObjectItem(result, "result")->valueint;
            total_count += count;
            cJSON_Delete(result);
        } else {
            printf("Cannot connect to %s:%d\n", ip, port);
        }

        close(sock);
        cJSON_Delete(task);
        free(task_str);
    }

    gettimeofday(&end, NULL);
    long secs = end.tv_sec - begin.tv_sec;
    long usecs = end.tv_usec - begin.tv_usec;
    double time = secs + usecs / 1000000.0;

    printf("Tasks completed, found %d primes, taking %.3f s\n", total_count, time);
    return NULL;
}
