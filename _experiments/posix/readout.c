#include <pthread.h>
#include <stdio.h>

int main() {
    printf("Sizeof(pthread_t) = %zu\n", sizeof(pthread_t));
    return 0;
}
