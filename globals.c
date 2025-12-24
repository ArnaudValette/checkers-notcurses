#include "globals.h"

pthread_mutex_t poll_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t poll_cv = PTHREAD_COND_INITIALIZER;
int stop_exec_mutex = 0;
int ui_dirty_mutex = 1;
