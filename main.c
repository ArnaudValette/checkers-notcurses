#include "main.h"
#include <bits/pthreadtypes.h>
#include <locale.h>
#include <math.h>
#include <notcurses/nckeys.h>
#include <notcurses/notcurses.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t poll_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t poll_cv = PTHREAD_COND_INITIALIZER;
int stop_exec = 0;
int ui_dirty = 1;

typedef struct input_handler_arg {
  struct notcurses *nc;

} input_handler_arg;

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

    usleep(10);
  }

  return NULL;
}

int main(int c, char **v) {
  (void)c;
  (void)v;

  int code = 0;
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

  code = notcurses_mice_enable(nc, NCMICE_MOVE_EVENT | NCMICE_BUTTON_EVENT);
  _CHECK((code == -1), code = -3);

  notcurses_render(nc);

  pthread_t t;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  struct input_handler_arg arg = {.nc = nc};
  pthread_create(&t, &attr, handle_input, &arg);
  pthread_attr_destroy(&attr);

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

struct notcurses *init() {
  setlocale(LC_ALL, "");
  /*
   * Should you trust nc automatic detection ?
   * char *term = getenv("TERM");
   * opts->termtype = term;
   */
  notcurses_options opts = {.flags = NCOPTION_SUPPRESS_BANNERS,
                            .loglevel = NCLOGLEVEL_SILENT};
  return notcurses_init(&opts, stdout);
}

struct ncvisual *generate_board(V2 dim) {
  uint32_t *buff = malloc(dim.y * dim.x * sizeof(uint32_t));
  if (buff == NULL) return NULL;
  int cellsz = dim.x / 8;
  for (ui y = 0; y < dim.y; y++) {
    for (ui x = 0; x < dim.x; x++) {
      uint32_t col =
          ((y / cellsz) % 2 == (x / cellsz) % 2) ? 0xFF000000 : 0xFFFFFFFF;
      buff[x + (y * dim.x)] = col;
    }
  }
  struct ncvisual *nc = ncvisual_from_rgba(buff, dim.y, dim.x * 4, dim.x);
  if (nc == NULL) return NULL;
  free(buff);
  return nc;
}

struct ncplane *stdplane_util(struct notcurses *nc, unsigned int *y,
                              unsigned int *x, unsigned int *celly,
                              unsigned int *cellx) {
  struct ncplane *stdplane = notcurses_stddim_yx(nc, y, x);
  if (stdplane == NULL) return NULL;
  ncplane_pixel_geom(stdplane, NULL, NULL, celly, cellx, NULL, NULL);
  return stdplane;
}

Stamp *stamp(struct ncplane *root, struct ncvisual *(*f)(V2), V2 pos, V2 spx,
             V2 sz) {
  Stamp *s = malloc(sizeof(Stamp));
  s->popts = (struct ncplane_options){.y = pos.y,
                                      .x = pos.x,
                                      .rows = spx.y / sz.y,
                                      .cols = spx.x / sz.x,
                                      .flags = 0};
  s->plane = ncplane_create(root, &s->popts);

  s->vopts = (struct ncvisual_options){
      .n = s->plane, .scaling = NCSCALE_STRETCH, .blitter = NCBLIT_PIXEL};
  s->visual = f(spx);
  return s;
}

void blit_stamp(struct notcurses *nc, Stamp *s) {
  ncvisual_blit(nc, s->visual, &s->vopts);
}

void free_stamp(Stamp *s) {
  if (s != NULL) {
    if (s->plane != NULL) ncplane_destroy(s->plane);
    if (s->visual != NULL) ncvisual_destroy(s->visual);
    free(s);
  }
}
