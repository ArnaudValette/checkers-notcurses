#ifndef DRAW_H
#define DRAW_H
#include "ncapi.h"
#include "types.h"
#include <stdint.h>


void draw_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz);
void draw_board(FrameBuffer *fb, ui cellsPerRow);

void draw_circle(FrameBuffer *fb, uint32_t color, ui r, V2 pos);
void draw_pawn(FrameBuffer *fb, uint32_t color, ui r, V2 pos, ui cellsPerRow);

#endif
