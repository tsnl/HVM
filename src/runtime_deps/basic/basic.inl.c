#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum {
    HVM_EXIT_CODE_THREAD_OP_FAILED
} HvmExitCode;

inline static void panic(HvmExitCode ec, char const* fmt, ...) {
    fprintf(stderr, "PANIC: ");
    {
        va_list arg_stack_ptr;
        va_start(arg_stack_ptr, fmt);
        vfprintf(stderr, fmt, arg_stack_ptr);
        va_end(arg_stack_ptr);
    }
    fprintf(stderr, "\n");
    fflush(stderr);
    exit(ec);
}
