#ifndef DRAW_H
#define DRAW_H
#include "ncapi.h"
#include "types.h"
#include <stdint.h>

#define BLACK_CELL (0xFF330033)
#define WHITE_CELL (0xFFAABBAA)

#define WHITE_KING (0xFFFFAAFF)
#define CURR_WHITE (0xFFFF8833)
#define WHITE_PAWN (0xFFFFFFFF)

#define BLACK_PAWN (0xFF003388)
#define BLACK_KING (0xFF008888)
#define CURR_BLACK (0xFF0033FF)

#define SPRITE_CURR_WHITE_KING 3
#define SPRITE_WHITE_KING 2
#define SPRITE_CURR_WHITE 1
#define SPRITE_WHITE_PAWN 0

#define SPRITE_CURR_BLACK_KING 7
#define SPRITE_BLACK_KING 6
#define SPRITE_CURR_BLACK 5
#define SPRITE_BLACK_PAWN 4

void draw_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz, ui border_width);

void fill_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz);
void draw_board(FrameBuffer *fb, ui cellsPerRow);

void draw_circle(FrameBuffer *fb, uint32_t color, ui r, V2 pos);
void draw_pawn(FrameBuffer *fb, uint32_t color, ui r, V2 pos, ui cellsPerRow);
void draw_pawn_sprite(FrameBuffer *fb, ui pawn_value ,V2 pos, ui cellsPerRow);

void draw_reach(FrameBuffer *fb, uint32_t color, V2 cellPos, ui cellsPerRow);

bool init_spritesheet();

#endif
