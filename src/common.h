#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

#define MAX_PEERS 10
#define BUFFER_SIZE 2048
#define USEC_IN_SEC 1000000.0   // 1e6
#define PROJECT_NAME "Taskune"

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_CYAN    "\033[1;36m"

static inline void info(const char* msg) {
    printf(COLOR_GREEN "[INFO] %s" COLOR_RESET "\n", msg);
}

static inline void task_info(const char* msg) {
    printf(COLOR_CYAN "[Worker] %s" COLOR_RESET "\n", msg);
}

static inline void warn(const char* msg) {
    printf(COLOR_YELLOW "[WARN] %s" COLOR_RESET "\n", msg);
}

static inline void error(const char* msg) {
    printf(COLOR_RED "[ERROR] %s" COLOR_RESET "\n", msg);
}

#endif // COMMON_H
