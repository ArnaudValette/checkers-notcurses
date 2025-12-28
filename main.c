#include "main.h"
#include "board.h"
#include "draw.h"
#include "globals.h"
#include "input.h"
#include "ncapi.h"
#include "types.h"
#include <bits/pthreadtypes.h>
#include <notcurses/nckeys.h>
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CELLS (10)

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

  ui x, y, cX, cY;
  struct notcurses *nc = NULL;
  struct ncplane *stdplane = NULL;
  FrameBuffer fb = {.buf = NULL, .dim = v2(0)};
  Stamp *board = NULL;
  Stamp *warn = NULL;
  u8 *game_board = getBoard();
  (void)warn;

  if ((nc = init()) == NULL) exit(-1);

  initBoard();
  if (!prepare_fb(&fb, v2(600))) goto ret;

  stdplane = stdplane_util(nc, &y, &x, &cY, &cX);

  draw_board(&fb, N_CELLS);
  for (int i = 0; i < 100; i++) {
    int y = i / 10;
    int x = i % 10;
    u8 val = game_board[i];
    if (val == 1) {
      draw_pawn(&fb, 0xFF00FFFF, 20, V2(y, x), N_CELLS);
    } else if (val == 2) {
      draw_pawn(&fb, 0xFF0000FF, 20, V2(y, x), N_CELLS);
    }
  }
  board = stamp(stdplane, &fb, v2(0), V2(cY, cX));
  if (board == NULL) goto ret;
  blit_stamp(nc, board);

  notcurses_render(nc);

  iTHREAD(t, attr_t, arg_t, {.nc = nc});
  rTHREAD(t, attr_t, handle_input, arg_t);

  // double rotation = 0.1;
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
    // ncvisual_rotate(board->visual, rotation);
    // rotation += 0.1;
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
