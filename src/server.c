#include "task.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "common.h"

void* server_thread(void* arg) {
    int port = *(int*)arg;
    int server_fd, new_socket;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("[Server] Listening on port %d...\n", port);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
        char buffer[2048] = {0};
        read(new_socket, buffer, sizeof(buffer));

        // Parse task JSON
        cJSON* task = cJSON_Parse(buffer);
        const char* type = cJSON_GetObjectItem(task, "type")->valuestring;

        if (strcmp(type, "prime") == 0) {
            int start = cJSON_GetObjectItem(task, "start")->valueint;
            int end = cJSON_GetObjectItem(task, "end")->valueint;

            int count = 0;
            for (int i = start; i <= end; i++) {
                if (is_prime(i)) count++;
            }

            printf("[Worker] Tasks completed: there are %d primes between %d ~ %d\n", count, start, end);

            // Return results
            cJSON* reply = cJSON_CreateObject();
            cJSON_AddNumberToObject(reply, "result", count);
            char* reply_str = cJSON_PrintUnformatted(reply);
            send(new_socket, reply_str, strlen(reply_str), 0);
            cJSON_Delete(reply);
            free(reply_str);
        }

        cJSON_Delete(task);
        close(new_socket);
    }

    return NULL;
}
