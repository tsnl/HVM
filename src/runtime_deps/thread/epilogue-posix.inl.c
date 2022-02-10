// DEBUG: start '/src/runtime_deps/threads/impl-posix.inl.c'

#include <pthread.h>
#include <stdio.h>

typedef pthread_t Thd;
typedef void*(*ThdCb)(void*);
typedef pthread_mutex_t Mutex;
typedef pthread_cond_t CondVar;
inline static void thread_create(Thd* restrict thread, ThdCb start_routine, void* restrict arg);
inline static void thread_join(Thd thread, void** ret_val_ptr);
inline static void thread_mutex_init(Mutex* mutex);
inline static void thread_mutex_destroy(Mutex* mutex);
inline static void thread_mutex_lock(Mutex* mutex);
inline static void thread_mutex_unlock(Mutex* mutex);
inline static void thread_cond_init(CondVar* cond_var);
inline static void thread_cond_destroy(CondVar* cond_var);
inline static void thread_cond_signal(CondVar* cond_var);
inline static void thread_cond_wait(CondVar* cond_var, Mutex* mutex);

inline static void thread_create(Thd* restrict thread, ThdCb start_routine, void* restrict arg) {
  __thread_check(pthread_create(thread, NULL, start_routine, arg), "pthread_create failed");
}
inline static void thread_join(Thd thread, void** ret_val_ptr) {
  __thread_check(pthread_join(thread, ret_val_ptr), "pthread_join failed");
}
inline static void thread_mutex_init(Mutex* mutex) {
  __thread_check(pthread_mutex_init(mutex, NULL), "pthread_mutex_init failed");
}
inline static void thread_mutex_destroy(Mutex* mutex) {
  __thread_check(pthread_mutex_destroy(mutex), "pthread_mutex_destroy failed");
}
inline static void thread_mutex_lock(Mutex* mutex) {
  __thread_check(pthread_mutex_lock(mutex), "pthread_mutex_lock failed");
}
inline static void thread_mutex_unlock(Mutex* mutex) {
  __thread_check(pthread_mutex_unlock(mutex), "pthread_mutex_unlock failed");
}
inline static void thread_cond_init(CondVar* cond_var) {
  __thread_check(pthread_cond_init(cond_var, NULL), "pthread_cond_init failed");
}
inline static void thread_cond_destroy(CondVar* cond_var) {
  __thread_check(pthread_cond_destroy(cond_var), "pthread_cond_destroy failed");
}
inline static void thread_cond_signal(CondVar* cond_var) {
  __thread_check(pthread_cond_signal(cond_var), "pthread_cond_signal failed");
}
inline static void thread_cond_wait(CondVar* cond_var, Mutex* mutex) {
  __thread_check(pthread_cond_wait(cond_var, mutex), "pthread_cond_wait failed");
}
