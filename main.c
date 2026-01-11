#include "main.h"
#include "board.h"
#include "draw.h"
#include "globals.h"
#include "input.h"
#include "logic.h"
#include "ncapi.h"
#include "types.h"
#include <notcurses/nckeys.h>
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CELLS (10)

void draw(FrameBuffer *fb) {
  u8 *game_board = getBoard();
  draw_board(fb, N_CELLS);
  for (int i = 0; i < 100; i++) {
    int y = i / N_CELLS;
    int x = i % N_CELLS;
    u8 val = game_board[i];
    if (val > 0) {
      //draw_pawn(fb, handleColor(x, y, val), 20, V2(y, x), N_CELLS);
      draw_pawn_sprite(fb, handlePawnType(x,y,val), V2(y,x), N_CELLS);
    }
    if (i != getCurrPawn()) {
      if (isReachable(i % 10, i / 10)) {
        int col = i % 10;
        int row = i / 10;
        draw_reach(fb, 0xFF00AAAA, V2(row, col), 10);
      }
    }
  }
}

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

  bool thread_init=false;
  ui x, y, cX, cY;
  struct notcurses *nc = NULL;
  struct ncplane *stdplane = NULL;
  FrameBuffer fb = {.buf = NULL, .dim = v2(0)};
  Stamp *board = NULL;
  Stamp *warn = NULL;
  u8 *game_board = getBoard();
  (void)warn;

  if ((nc = init()) == NULL) exit(-1);

  init_spritesheet();
  initBoard(10, 10);
  if(!initRules()) goto ret;
  if (!prepare_fb(&fb, v2(600))) goto ret;

  stdplane = stdplane_util(nc, &y, &x, &cY, &cX);

  draw(&fb);
  board = stamp(stdplane, &fb, v2(0), V2(cY, cX));
  if (board == NULL) goto ret;
  blit_stamp(nc, board);

  V2 dims = V2(0, 0);
  V2 cSz = V2(cY, cX);
  ncvgeom visgeom;
  ncvisual_geom(nc, board->visual, &board->vopts, &visgeom);
  dims.x = visgeom.rcellx * cX;
  dims.y = visgeom.rcelly * cY;
  notcurses_render(nc);

  struct input_handler_arg args = {.nc = nc, .cell_size = cSz, .dims = dims};
  iTHREAD(t, attr_t);
  rTHREAD(t, attr_t, handle_input, args);
  thread_init=true;

  pthread_mutex_lock(&poll_mtx);
  while (!stop_exec_mutex) {

    /* Wait for modifying inputs (ui_dirty) */
    while (!stop_exec_mutex && !ui_dirty_mutex) {
      pthread_cond_wait(&poll_cv, &poll_mtx);
    }

    if (stop_exec_mutex) {
      break;
    }
    ui_dirty_mutex = 0;
    pthread_mutex_unlock(&poll_mtx);
    draw(&fb);
    replace_stamp_buffer(board, &fb);
    blit_stamp(nc, board);
    notcurses_render(nc);
    pthread_mutex_lock(&poll_mtx);
  }
  pthread_mutex_unlock(&poll_mtx);

  goto ret;

ret:
  if(thread_init) pthread_join(t, NULL);
  free_stamp(board);
  if (game_board != NULL) freeBoard();
  if (nc != NULL) notcurses_stop(nc);
  if (fb.buf != NULL) free_fb(&fb);
  destroy_rules();
  printf("Gracefully shutdown...\n");
  return 0;
}
