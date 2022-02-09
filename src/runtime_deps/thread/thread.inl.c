typedef void*(*ThdCb)(void*);
// provides: thread_{create, join} with `Thd`, `thread_create`, `thread_join`
// TODO: replace pthread_mutex_{t, init, destroy, lock, unlock} with `Mutex`, `thread_mutex_...`
// TODO: replace pthread_cond_{t, init, destroy, signal, wait} with `Cond`, `thread_cond_...`
