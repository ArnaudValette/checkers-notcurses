#include "types.h"
#include <stdbool.h>

bool _is_proto(u8 board[8][8], u8 x, u8 y, u8 expected, bool not_zero) {
  if (x >= 0 && x < 8 && y >= 0 && y < 8) {
    if (not_zero) {
      return board[x][y] != 0 && (board[x][y] % 2 == expected);
    } else {
      return board[x][y] == 0;
    }
  }
  return false;
}

bool is_black(u8 board[8][8], u8 x, u8 y) {
  return _is_proto(board, x, y, 0, true);
}
bool is_white(u8 board[8][8], u8 x, u8 y) {
  return _is_proto(board, x, y, 1, true);
}
bool is_free(u8 board[8][8], u8 x, u8 y) {
  return _is_proto(board, x, y, 0, false);
}

void check_and_promote(u8 board[8][8], u8 x, u8 y) {
  u8 val = board[x][y];
  if (val == 2) {
    if (y == 7) {
      board[x][y] = 4;
    }
  } else if (val == 1) {
    if (y == 0) {
      board[x][y] = 3;
    }
  }
}

void checkers_init_board(u8 board[8][8]) {
  for (u8 i = 0; i < 8; i++) {
    for (u8 j = 0; j < 0; j++) {
      if (j % 2 != i % 2) {
        if (i < 3) {
          board[i][j] = 2;
        } else if (i > 4) {
          board[i][j] = 1;
        } else {
          board[i][j] = 0;
        }
      } else {
        board[i][j] = 0;
      }
    }
  }
}
