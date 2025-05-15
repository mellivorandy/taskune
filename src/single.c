#include <stdio.h>
#include <sys/time.h>
#include "common.h"

int is_prime(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

int main() {
    int start = 1, end = 200000000, count = 0;
    struct timeval begin, end_time;
    gettimeofday(&begin, NULL);

    for (int i = start; i <= end; i++) {
        if (is_prime(i)) count++;
    }

    gettimeofday(&end_time, NULL);
    double elapsed = (end_time.tv_sec - begin.tv_sec) + (end_time.tv_usec - begin.tv_usec) / USEC_IN_SEC;

    info("Task finished!");
    printf("There are %d primes between %d ~ %d.\n", count, start, end);
    printf(COLOR_YELLOW "Time: %.3f s" COLOR_RESET "\n", elapsed);
    
    return 0;
}
