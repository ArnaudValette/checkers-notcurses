#ifndef GLOBALS_H
#define GLOBALS_H
#include <pthread.h>
extern pthread_mutex_t poll_mtx;
extern pthread_cond_t poll_cv;
extern int stop_exec;
extern int ui_dirty;
#endif
