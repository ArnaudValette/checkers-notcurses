#include "board.h"
#include "draw.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

/* TODO: use bytefields */
static u8 *board;
static int currPlayer = 1;
static int currPawn = -1;
static bool currPawnIsKing = false;

void setKingPawn(bool b) { currPawnIsKing = b; }
int getCurrPawn() { return currPawn; }
void setCurrPawn(int i) { currPawn = i; }
int getCurrPlayer() { return currPlayer; }

u8 *getBoard() { return board; }
void setBoard(u8 cell, u8 value) { board[cell] = value; }

int getOpponent() { return (currPlayer % 2) + 1; }

bool isOpponentPawnOrKing(int value) {
  if (currPlayer == 1) {
    return value == 2 || value == 4;
  } else {
    return value == 1 || value == 3;
  }
}

void nextPlayer() { currPlayer = getOpponent(); }

bool isPlayerPawn(u8 col, u8 row) {
  return board[row * 10 + col] > 0 &&
         board[row * 10 + col] % 2 == currPlayer % 2;
}

bool isKingPawn(u8 col, u8 row) { return board[row * 10 + col] > 2; }
bool isCurrentPawn(int col, int row) { return ((row * 10) + col) == currPawn; }

u8 getRow(u8 idx) { return idx / 10; }
u8 getCol(u8 idx) { return idx % 10; }

u8 getCell(int col, int row) {
  if (col >= 10 || col < 0 || row >= 10 || row < 0) {
    return -1;
  }
  return board[col + row * 10];
}
bool isIdxBlack(u8 idx) { return getRow(idx) % 2 != getCol(idx) % 2; }
bool isIdxTopTerritory(u8 idx) { return getRow(idx) < 4; }
bool isIdxBotTerritory(u8 idx) { return getRow(idx) > 5; }

void initBoard(ui col, ui row) {
  board = malloc(col * row * sizeof(u8));
  for (ui i = 0; i < col * row; i++) {
    if (isIdxBlack(i)) {
      board[i] = isIdxBotTerritory(i) ? 1 : isIdxTopTerritory(i) ? 2 : 0;
    } else {
      board[i] = 0;
    }
  }
}

void freeBoard() { free(board); }

uint32_t handlePawnType(int col, int row, u8 player) {
  bool isCurr = isCurrentPawn(col, row);
  bool isKing = isKingPawn(col, row);
  uint32_t color;
  if (player % 2 == 1) {
    color = SPRITE_WHITE_PAWN;
    if (isKing && isCurr) {
      color = SPRITE_CURR_WHITE_KING;
    } else if (isCurr) {
      color = SPRITE_CURR_WHITE;
    } else if (isKing) {
      color = SPRITE_WHITE_KING;
    }
  } else {
    color = SPRITE_BLACK_PAWN;
    if (isKing && isCurr) {
      color = SPRITE_CURR_BLACK_KING;
    } else if (isCurr) {
      color = SPRITE_CURR_BLACK;
    } else if (isKing) {
      color = SPRITE_BLACK_KING;
    }
  }
  return color;
}

uint32_t handleColor(int col, int row, u8 player) {
  bool isCurr = isCurrentPawn(col, row);
  bool isKing = isKingPawn(col, row);
  uint32_t color;
  if (player % 2 == 1) {
    color = WHITE_PAWN;
    if (isCurr) color = CURR_WHITE;
    if (isKing) color = WHITE_KING;

  } else {
    color = BLACK_PAWN;
    if (isCurr) color = CURR_BLACK;
    if (isKing) color = BLACK_KING;
  }
  return color;
}

void movePawn(int s, int t) {
  board[t] = board[s];
  board[s] = 0;
}

void handlePromotion() {
  if (currPlayer == 1) {
    if (currPawn / 10 == 0) {
      board[currPawn] += 2;
    }
  } else {
    if (currPawn / 10 == 9) {
      board[currPawn] += 2;
    }
  }
}
