#include "input.h"
#include "debug.h"
#include "globals.h"
#include "interaction.h"
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdbool.h>


bool ui_changed(ncinput *ni, V2 cell_size, V2 dims) {
  return handle_actions(ni, cell_size, dims);
}

void *handle_input(void *_arg) {
  input_handler_arg *ctx = _arg;
  ncinput ni;
  while (1) {
    pthread_mutex_lock(&poll_mtx);
    if (stop_exec_mutex) {
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
    if ((ni.id == NCKEY_ESC && ni.evtype == NCTYPE_RELEASE) || stop_exec_mutex || ni.id == 'q') {
      stop_exec_mutex = 1;
      pthread_cond_broadcast(&poll_cv);
      pthread_mutex_unlock(&poll_mtx);
      break;
    }

    if (ui_changed(&ni, ctx->cell_size, ctx->dims)) {
      ui_dirty_mutex = 1;
      debug_log("ui_dirty triggered by event\n");
      pthread_cond_broadcast(&poll_cv);
    }
    pthread_mutex_unlock(&poll_mtx);
  }

  return NULL;
}

