#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum {
  HVM_EXIT_CODE_ALL_OK = 0,
  HVM_EXIT_CODE_THREAD_OP_FAILED,
  HVM_EXIT_CODE_TIME_OP_FAILED
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
inline static void __check(char const* prefix, int res, char const* more, int exit_code) {
  if (res != 0) {
    // panic(
    //   exit_code, 
    //   "%s: %s: errno %d",  
    //   prefix, more, res
    // );
    fprintf(stderr,
      "%s: %s: errno %d: [not (for debug)] exiting with ec=%d...\n",  
      prefix, more, res, exit_code
    );
  }
}
inline static void __thread_check(int res, char const* more) {
  __check("THREAD", res, more, HVM_EXIT_CODE_THREAD_OP_FAILED);
}
inline static void __time_check(int res, char const* more) {
  __check("TIME", res, more, HVM_EXIT_CODE_TIME_OP_FAILED);
}
