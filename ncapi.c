#include "ncapi.h"
#include "types.h"
#include <locale.h>
#include <notcurses/notcurses.h>
#include <stdint.h>

ncblitter_e selected_blit = NCBLIT_DEFAULT;

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
  ncpixelimpl_e px_support = notcurses_check_pixel_support(nc);
  if (px_support != NCPIXEL_NONE) {
    selected_blit = NCBLIT_PIXEL;
  }
  if (nc == NULL) return NULL;
  int code = notcurses_mice_enable(nc, NCMICE_MOVE_EVENT | NCMICE_BUTTON_EVENT);
  if (code == -1) return NULL;
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

Stamp *stamp(struct ncplane *root, FrameBuffer *buffer, V2 pos,
             V2 size_cols_rows) {
  Stamp *s = malloc(sizeof(Stamp));
  s->popts = (struct ncplane_options){.y = pos.y / size_cols_rows.y,
                                      .x = pos.x / size_cols_rows.x,
                                      .rows = buffer->dim.y / size_cols_rows.y,
                                      .cols = buffer->dim.x / size_cols_rows.x,
                                      .flags = 0};
  s->plane = ncplane_create(root, &s->popts);

  s->visual = NULL;
  if (replace_stamp_buffer(s, buffer) == NULL) return NULL;
  return s;
}

Stamp *replace_stamp_buffer(Stamp *s, FrameBuffer *buffer) {
  if (s->visual != NULL) {
    ncvisual_destroy(s->visual);
  }
  s->vopts = (struct ncvisual_options){
      .n = s->plane, .scaling = NCSCALE_SCALE_HIRES, .blitter = selected_blit};

  struct ncvisual *nc = ncvisual_from_rgba(buffer->buf, buffer->dim.y,
                                           buffer->dim.x * 4, buffer->dim.x);
  if (nc == NULL) return NULL;
  s->visual = nc;
  return s;
}

void blit_stamp(struct notcurses *nc, Stamp *s) {
  ncvisual_blit(nc, s->visual, &s->vopts);
}

void free_stamp(Stamp *s) {
  if (s != NULL) {
    if (s->plane != NULL) {
      ncplane_destroy(s->plane);
      s->plane = NULL;
    }
    if (s->visual != NULL) {
      ncvisual_destroy(s->visual);
      s->visual = NULL;
    }
    free(s);
    s = NULL;
  }
}
