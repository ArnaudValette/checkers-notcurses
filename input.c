#include "input.h"
#include "globals.h"
#include <pthread.h>
#include <stdbool.h>


bool ui_changed(ncinput *ni) {
  if (ni->id == NCKEY_ENTER) {
    return true;
  } else {
    return false;
  }
}

void *handle_input(void *_arg) {
  input_handler_arg *ctx = _arg;
  ncinput ni;
  while (1) {
    pthread_mutex_lock(&poll_mtx);
    if (stop_exec) {
      pthread_cond_broadcast(&poll_cv);
      pthread_mutex_unlock(&poll_mtx);
      break;
    }
    pthread_mutex_unlock(&poll_mtx);

    notcurses_get_blocking(ctx->nc, &ni);

    /* TODO: this is a hack as for now notcurses consider
       mouse moving (y<0 || x<0) to be an ESC press on kitty and ghostty
     */
    pthread_mutex_lock(&poll_mtx);
    if ((ni.id == NCKEY_ESC && ni.evtype == NCTYPE_RELEASE) || stop_exec) {
      stop_exec = 1;
      pthread_cond_broadcast(&poll_cv);
      pthread_mutex_unlock(&poll_mtx);
      break;
    }

    if (ui_changed(&ni)) {
      ui_dirty = 1;
      pthread_cond_broadcast(&poll_cv);
    }
    pthread_mutex_unlock(&poll_mtx);
  }

  return NULL;
}

