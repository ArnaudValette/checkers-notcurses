#include "draw.h"
#include "types.h"
#include <stdint.h>

void draw_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz) {
  ui bound = fb->dim.x * fb->dim.y;
  for (ui y = pos.y; y < (pos.y + sz.y); y++) {
    ui _y = (fb->dim.x * y);
    for (ui x = pos.x; x < (pos.x + sz.x); x++) {
      ui location = _y + x;
      if (location < bound) {
        fb->buf[location] = color;
      }
    }
  }
}

void draw_board(FrameBuffer *fb, ui cellsPerRow) {
  ui min = (fb->dim.x < fb->dim.y) ? fb->dim.x : fb->dim.y;
  ui cellSize = min / cellsPerRow;
  for (ui y = 0; y < cellsPerRow; y++) {
    for (ui x = 0; x < cellsPerRow; x++) {
      uint32_t color;
      if (y % 2 == x % 2) {
        color = 0xFF000000;
      } else {
        color = 0xFFFFFFFF;
      }
      draw_rectangle(fb, color, V2(y * cellSize, x * cellSize), v2(cellSize));
    }
  }
}

/* FrameBuffer *generate_pawn() { */
/*   gen_pawn_args *args = _args; */
/*   ui radius = args->radius; */
/*   V2 sz = args->container_sz; */
/*   ui x = sz.x; */
/*   ui y = sz.y; */
/*   uint32_t *buffer = malloc(sz.y * sz.x * sizeof(uint32_t)); */
/*   if (buffer == NULL) return NULL; */
/*   ui xcenter = x / 2; */
/*   ui ycenter = y / 2; */

/*   memset(buffer, 0, sz.y * sz.x * sizeof(uint32_t)); */
/*   ui r_square = radius * radius; */
/*   for (ui j = ycenter - radius; j < ycenter + radius; j++) { */
/*     ui dy = (ycenter - j) * (ycenter - j); */
/*     for (ui i = xcenter - radius; i < xcenter + radius; i++) { */
/*       ui dx = (xcenter - i) * (xcenter - i); */
/*       if (dx + dy < r_square) { */
/*         buffer[j * x + i] = 0xFF0000FF; */
/*       } */
/*     } */
/*   } */
/* } */
