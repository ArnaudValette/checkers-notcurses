#include "draw.h"
#include "ncapi.h"
#include "types.h"
#include <assert.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Game § Draw → initializers & utils                                       ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  
static uint32_t sprites[8][60 * 60];

bool init_spritesheet() {
  int w, h, n;
  unsigned char *spritesheet = stbi_load("Sprite-0001.png", &w, &h, &n, 4);
  assert(w == 60 * 8);
  assert(h == 60);
  for (ui sprite = 0; sprite < 8; sprite++) {
    for (ui j = 0; j < 60; j++) {
      for (ui i = 0; i < 60; i++) {
        int pos = (i + (60 * sprite)) + (60 * 8) * j;
        uint8_t r = spritesheet[pos * 4];
        uint8_t g = spritesheet[pos * 4 + 1];
        uint8_t b = spritesheet[pos * 4 + 2];
        uint8_t a = spritesheet[pos * 4 + 3];
        sprites[sprite][i + j * 60] = (a << 24) | (b << 16) | (g << 8) | (r);
      }
    }
  }
  return true;
}

ui get_cellSize(FrameBuffer *fb, ui cellsPerRow) {
  ui min = (fb->dim.x < fb->dim.y) ? fb->dim.x : fb->dim.y;
  return min / cellsPerRow;
}

/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Game § Draw → sprites & shapes                                           ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  
void draw_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz,
                    ui border_width) {
  ui bound = fb->dim.x * fb->dim.y;
  for (ui y = pos.y; y < (pos.y + sz.y); y++) {
    ui _y = (fb->dim.x * y);
    for (ui x = pos.x; x < (pos.x + sz.x); x++) {
      ui location = _y + x;
      if (location < bound) {
        if (x <= pos.x + border_width || x >= ((pos.x + sz.x) - border_width) ||
            y <= pos.y + border_width || y >= (pos.y + sz.y - border_width)) {
          fb->buf[location] = color;
        }
      }
    }
  }
}

void fill_rectangle(FrameBuffer *fb, uint32_t color, V2 pos, V2 sz) {
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
  ui cellSize = get_cellSize(fb, cellsPerRow);
  for (ui y = 0; y < cellsPerRow; y++) {
    for (ui x = 0; x < cellsPerRow; x++) {
      uint32_t color;
      if (y % 2 == x % 2) {
        color = WHITE_CELL;
      } else {
        color = BLACK_CELL;
      }
      fill_rectangle(fb, color, V2(y * cellSize, x * cellSize), v2(cellSize));
    }
  }
}

void draw_circle(FrameBuffer *fb, uint32_t color, ui r, V2 pos) {
  ui dr = r * r;
  for (ui y = pos.y - r; y < pos.y + r; y++) {
    ui dy = (y - pos.y) * (y - pos.y);
    ui _y = (fb->dim.x * y);
    for (ui x = pos.x - r; x < pos.x + r; x++) {
      ui dx = (x - pos.x) * (x - pos.x);
      ui location = _y + x;
      if (dx + dy < dr) {
        fb->buf[location] = color;
      }
    }
  }
}

void draw_pawn(FrameBuffer *fb, uint32_t color, ui r, V2 cellPos,
               ui cellsPerRow) {
  ui cellSize = get_cellSize(fb, cellsPerRow);
  ui xPos = (cellPos.x * cellSize) + (cellSize / 2);
  ui yPos = (cellPos.y * cellSize) + (cellSize / 2);
  draw_circle(fb, color, r, V2(yPos, xPos));
}

void draw_pawn_sprite(FrameBuffer *fb, ui pawn_value, V2 pos, ui cellsPerRow) {
  ui cellSize = get_cellSize(fb, cellsPerRow);
  ui xPos = (pos.x * cellSize);
  ui yPos = (pos.y * cellSize);
  for (int j = 0; j < 60; j++) {
    for (int i = 0; i < 60; i++) {
      uint32_t color = sprites[pawn_value][i + j * 60];
      if (color & 0xFF000000) {
        fb->buf[(xPos + i) + ((j + yPos) * 600)] = color;
      }
    }
  }
}

void draw_reach(FrameBuffer *fb, uint32_t color, V2 cellPos, ui cellsPerRow) {
  ui cellSize = fb->dim.x / cellsPerRow;
  draw_rectangle(fb, color, V2(cellPos.y * cellSize, cellPos.x * cellSize),
                 v2(cellSize), 4);
}
