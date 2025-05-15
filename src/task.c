#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "task.h"

int is_prime(long long n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void handle_task(const char* json_str) {
    cJSON* task = cJSON_Parse(json_str);
    const char* type = cJSON_GetObjectItem(task, "type")->valuestring;

    if (strcmp(type, "prime") == 0) {
        long long start = cJSON_GetObjectItem(task, "start")->valueint;
        long long end = cJSON_GetObjectItem(task, "end")->valueint;
        long long count = 0;
        for (long long i = start; i <= end; i++) {
            if (is_prime(i)) count++;
        }
        printf("[Worker] Task finished: There are %lld primes between %lld ~ %lld\n", count, start, end);
    }

    cJSON_Delete(task);
}
