// see: on threads: https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
// see: on mutex: https://docs.microsoft.com/en-us/windows/win32/sync/using-mutex-objects
// see: on cv: https://docs.microsoft.com/en-us/windows/win32/sync/condition-variables

// FIXME: Windows ConditionVariable instances require an extra CRITICAL_SECTION object.
// Instead, use WaitOnAddress and implement a custom ConditionVariable.
// https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitonaddress
// Compare from: http://www.cs.kent.edu/~ruttan/sysprog/lectures/multi-thread/pthread_cond_init.html
// - pthread_cond_signal maps to WakeByAddressSingle
// - pthread_cond_wait maps to WaitOnAddress
// - problem is: what is 'the address'? what is 'the comparison value'? Cf comment root.
//   - address size: 

#include <stddef.h>
#include <stdio.h>
#include <windows.h>

typedef HANDLE Thd;
typedef DWORD(*ThdCb)(LPVOID);
typedef HANDLE Mutex;
typedef struct{CONDITION_VARIABLE cv;} CondVar;
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
inline static void __thread_check(int res, char const* more);

inline static void thread_create(Thd* restrict thread, ThdCb start_routine, void* restrict arg) {
    *thread = CreateThread(
        NULL,   // custom SECURITY_ATTRIBUTES: if NULL, cannot be inherited by subprocesses
        0,      // initial stack size: 0 indicates default stack size
        start_routine,
        NULL,   // user data pointer
        0,      // flags that control creation of the thread, e.g. start asleep
        NULL    // an output 'thread identifier': don't think we need this...
    );
    if (*thread == NULL) {
        __thread_check(GetLastError(), "thread_create: CreateThread failed");
    }
}
inline static void thread_join(Thd thread) {
    // The examples weren't straightforward, but I found this SO answer:
    // https://stackoverflow.com/questions/11779504/join-equivalent-in-windows
    // and the docs confirm it (see REMARKS)
    // https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject
    DWORD res = WaitForSingleObject(thread, INFINITE);
    if (res == WAIT_FAILED) {
        __thread_check(GetLastError(), "thread_join: WaitForSingleObject failed");
    }
}
inline static void thread_mutex_init(Mutex* mutex) {
    *mutex = CreateMutexA(
        NULL,   // custom SECURITY_ATTRIBUTES: if NULL, cannot be inherited by subprocesses
        FALSE,  // indicates we do not want ownership/lock of new mutex, like pthread_mutex_init
        NULL    // optional name... yes, a string name.
    );
    if (mutex == NULL) {
        __thread_check(GetLastError(), "thread_mutex_init: CreateMutexA failed");
    }
}
inline static void thread_mutex_destroy(Mutex* mutex) {
    BOOL success = CloseHandle(*mutex);
    if (!success) {
        __thread_check(GetLastError(), "thread_mutex_destroy: CloseHandle failed");
    }
}
inline static void thread_mutex_lock(Mutex* mutex) {
    DWORD res = WaitForSingleObject(*mutex, INFINITE);
    if (res == WAIT_FAILED) {
        __thread_check(GetLastError(), "thread_mutex_lock: WaitForSingleObject failed");
    }
}
inline static void thread_mutex_unlock(Mutex* mutex) {
    BOOL success = ReleaseMutex(*mutex);
    if (!success) {
        __thread_check(GetLastError(), "thread_mutex_unlock: ReleaseMutex failed");
    }
}
inline static void thread_cond_init(CondVar* cond_var) {
    // todo!
}
inline static void thread_cond_destroy(CondVar* cond_var) {
    // todo!
}
inline static void thread_cond_signal(CondVar* cond_var) {
    // todo!
}
inline static void thread_cond_wait(CondVar* cond_var, Mutex* mutex) {
    // todo!
}
