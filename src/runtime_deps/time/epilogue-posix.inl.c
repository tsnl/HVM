#include <sys/time.h>
#include <stddef.h>

typedef struct timeval timeval;

inline static void time_now(timeval* tp);
inline static void __time_check(int res, char const* more);

void time_now(timeval* tp) {
    int rc = gettimeofday(tp, NULL);
    __time_check(rc, "time_now: gettimeofday failed");
}