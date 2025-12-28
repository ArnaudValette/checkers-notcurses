#include "board.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/* TODO: use bytefields */
static u8 board[100] = {};
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

void initBoard() {
  for (int i = 0; i < 100; i++) {
    if (isIdxBlack(i)) {
      board[i] = isIdxBotTerritory(i) ? 1 : isIdxTopTerritory(i) ? 2 : 0;
    } else {
      board[i] = 0;
    }
  }
}

uint32_t handleColor(int col, int row, u8 player) {
  bool isCurr = isCurrentPawn(col, row);
  bool isKing = isKingPawn(col, row);
  uint32_t color;
  if (player % 2 == 1) {
    color = 0xFFFFFFFF;
    if (isKing)
      color = 0xFFFFAAFF;
    if (isCurr) {
      color = 0xFFFF8833;
    }

  } else {
    color = 0xFF003388;
    if (isKing)
      color = 0xFF008888;
    if (isCurr) {
      color = 0xFF0033FF;
    }
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
