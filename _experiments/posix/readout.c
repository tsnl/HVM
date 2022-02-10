#include <pthread.h>
#include <stdio.h>

int main() {
    printf("Sizeof(pthread_t) = %zu\n", sizeof(pthread_t));
#if defined(__TINYC__)
    printf("Running on TinyC\n", sizeof(pthread_t));
#endif
    return 0;
}
