#include "main.h"
#include "board.h"
#include "draw.h"
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

bool prepare_fb(FrameBuffer *fb, V2 sz) {
  fb->buf = malloc(sz.x * sz.y * sizeof(uint32_t));
  if (fb->buf == NULL) return 0;
  fb->dim = sz;
  return 1;
}

void free_fb(FrameBuffer *fb) { free(fb->buf); }

int main(int c, char **v) {
  (void)c;
  (void)v;

  unsigned int x, y, cX, cY;
  struct notcurses *nc = NULL;
  struct ncplane *stdplane = NULL;
  FrameBuffer fb = {.buf = NULL, .dim = v2(0)};
  Stamp *board = NULL;
  Stamp *warn = NULL;
  (void)warn;

  if ((nc = init()) == NULL) exit(-1);

  initBoard();
  if (!prepare_fb(&fb, v2(600))) goto ret;

  stdplane = stdplane_util(nc, &y, &x, &cY, &cX);

  draw_board(&fb, 10);
  board = stamp(stdplane, &fb, v2(0), V2(cY, cX));
  if (board == NULL) goto ret;
  blit_stamp(nc, board);

  notcurses_render(nc);

  iTHREAD(t, attr_t, arg_t, {.nc = nc});
  rTHREAD(t, attr_t, handle_input, arg_t);

  pthread_mutex_lock(&poll_mtx);
  while (!stop_exec_mutex) {

    while (!stop_exec_mutex && !ui_dirty_mutex) {
      pthread_cond_wait(&poll_cv, &poll_mtx);
    }

    if (stop_exec_mutex) {
      break;
    }

    ui_dirty_mutex = 0;
    pthread_mutex_unlock(&poll_mtx);
    replace_stamp_buffer(board, &fb);
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
  if (fb.buf != NULL) free_fb(&fb);
  printf("Gracefully shutdown...\n");
  return 0;
}
