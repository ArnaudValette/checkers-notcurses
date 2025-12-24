#include "main.h"
#include "globals.h"
#include "input.h"
#include "ncapi.h"
#include "types.h"
#include <bits/pthreadtypes.h>
#include <math.h>
#include <notcurses/nckeys.h>
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int c, char **v) {
  (void)c;
  (void)v;

  unsigned int x, y, cX, cY;
  struct notcurses *nc = NULL;
  struct ncplane *stdplane = NULL;
  Stamp *board = NULL;
  Stamp *warn = NULL;
  (void)warn;

  if ((nc = init()) == NULL) exit(-1);

  stdplane = stdplane_util(nc, &y, &x, &cY, &cX);
  board = stamp(stdplane, generate_board, v2(0), v2(600), V2(cY, cX));
  blit_stamp(nc, board);
  notcurses_render(nc);

  iTHREAD(t, attr_t, arg_t, {.nc = nc});
  rTHREAD(t, attr_t, handle_input, arg_t);

  pthread_mutex_lock(&poll_mtx);
  while (!stop_exec) {

    while (!stop_exec && !ui_dirty) {
      pthread_cond_wait(&poll_cv, &poll_mtx);
    }

    if (stop_exec) {
      break;
    }

    ui_dirty = 0;
    pthread_mutex_unlock(&poll_mtx);
    ncvisual_rotate(board->visual, (M_PI / 2.0));
    blit_stamp(nc, board);
    notcurses_render(nc);
    pthread_mutex_lock(&poll_mtx);
  }
  pthread_mutex_unlock(&poll_mtx);

  goto ret;

ret:
  pthread_join(t, NULL);
  free_stamp(board);
  if (nc != NULL) notcurses_stop(nc);
  printf("Gracefully shutdown...\n");
  return 0;
}
