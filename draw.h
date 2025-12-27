#ifndef DRAW_H
#define DRAW_H
#include "types.h"
#include <stdint.h>

typedef struct {
  uint32_t *buf;
  V2 dim;
} FrameBuffer;

void draw_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz);
void draw_board(FrameBuffer *fb, ui cellsPerRow);

#endif
