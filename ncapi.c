#include "ncapi.h"
#include "types.h"
#include <locale.h>
#include <notcurses/notcurses.h>

struct notcurses *init() {
  setlocale(LC_ALL, "");
  /*
   * Should you trust nc automatic detection ?
   * char *term = getenv("TERM");
   * opts->termtype = term;
   */
  notcurses_options opts = {.flags = NCOPTION_SUPPRESS_BANNERS,
                            .loglevel = NCLOGLEVEL_SILENT};
  struct notcurses *nc = notcurses_init(&opts, stdout);
  if(nc == NULL) return NULL;
  int code = notcurses_mice_enable(nc, NCMICE_MOVE_EVENT | NCMICE_BUTTON_EVENT);
  if(code == -1) return NULL;
  return nc;
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
