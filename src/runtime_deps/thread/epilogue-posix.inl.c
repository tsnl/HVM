// DEBUG: start '/src/runtime_deps/threads/impl-posix.inl.c'

#include <pthread.h>
#include <stdio.h>

typedef pthread_t Thd;
inline static void thread_create(Thd* restrict thread, ThdCb start_routine, void* restrict arg);
inline static void thread_join(Thd thread, void** ret_val_ptr);

inline static void thread_create(Thd* restrict thread, ThdCb start_routine, void* restrict arg) {
  int res = pthread_create(thread, NULL, start_routine, arg);
  if (res != 0) {
      panic(
        HVM_EXIT_CODE_THREAD_OP_FAILED, 
        "Could not create a thread: pthread_create failed with errno %d",  res
    );
  }
}
inline static void thread_join(Thd thread, void** ret_val_ptr) {
  int res = pthread_join(thread, ret_val_ptr);
  if (res != 0) {
    panic(
      HVM_EXIT_CODE_THREAD_OP_FAILED, 
      "Could not join a thread: pthread_join failed with errno %d", res
    );
  }
}
