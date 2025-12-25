#include "logic.h"
#include "board.h"
#include "types.h"
#include <bits/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* Max number of different paths in a checkers turn ? */
static bool reach[100] = {};
static Move kills[100] = {};

Move *getKillBuffer() { return kills; }

void resetReach() {
  for (int i = 0; i < 100; i++) {
    reach[i] = false;
  }
}

void resetKills() {
  for (int i = 0; i < 100; i++) {
    kills[i] = (Move){_NONMOVE, 0, NULL};
  }
}

bool isKillingMoveOption(u8 col, u8 row) {
  return kills[col + row * 10].cell != _NONMOVE;
}

bool isReachable(u8 col, u8 row) { return reach[col + row * 10]; }

void handleKillingMoves(u8 c, Move *prev) {
  u8 xi[] = {c - 11, c - 9, c + 11, c + 9};
  u8 ai[] = {c - 22, c - 18, c + 22, c + 18};
  bool flag[] = {0, 0, 0, 0};

  u8 _c = c % 10, _r = c / 10;
  if (_r - 2 >= 0) {
    if (_c - 2 >= 0)
      flag[0] = 1;
    if (_c + 2 < 10)
      flag[1] = 1;
  }
  if (_r + 2 < 10) {
    if (_c - 2 >= 0)
      flag[3] = 1;
    if (_c + 2 < 10)
      flag[2] = 1;
  }

  for (u8 i = 0; i < 4; i++) {
    if (flag[i]) {
      if (kills[ai[i]].cell == _NONMOVE && kills[ai[i]].cell != _PREKILL &&
          ai[i] != getCurrPawn()) {
        if (isOpponentPawnOrKing(getBoard()[xi[i]])) {
          if (getBoard()[ai[i]] == 0) {
            reach[ai[i]] = true;
            kills[ai[i]] = (Move){ai[i], xi[i], NULL};
            if (prev->cell != _NONMOVE)
              kills[ai[i]].prev = prev;
            handleKillingMoves(ai[i], &kills[ai[i]]);
          }
        }
      }
    }
  }
}

void handleKingKillingMoves(u8 c) {
  u8 col = c % 10, row = c / 10;
  KingMoveStateFlag flag[] = {_FREE, _FREE, _FREE, _FREE};
  Move directions[4];
  Move prekills[4];
  memset(prekills, 0, 4 * (sizeof(Move)));

  for (int i = 1; i < 10; i++) {
    KingHelper arr[] = {kingMove(col - i, row - i), kingMove(col + i, row - i),
                        kingMove(col + i, row + i), kingMove(col - i, row + i)};
    for (int j = 0; j < 4; j++) {
      if (arr[j].value != -1) {
        if (flag[j] == _FREE) {
          if (isOpponentPawnOrKing(arr[j].value)) {
            flag[j] = _ENEMY;
            directions[j].kill = arr[j].cell;
          } else if (arr[j].value != 0) {
            flag[j] = _FAIL;
          } else {
            reach[arr[j].cell] = true;
            prekills[j].cell = arr[j].cell;
            prekills[j].kill = _PREKILL;
          }
        } else if (flag[j] == _ENEMY) {
          if (arr[j].value == 0) {
            flag[j] = _SUCCESS;
            reach[arr[j].cell] = true;
            kills[arr[j].cell].cell = arr[j].cell;
            kills[arr[j].cell].kill = directions[j].kill;
            kills[arr[j].cell].prev = NULL;
            if (prekills[j].kill == _PREKILL) {
              kills[prekills[j].cell].cell = _PREKILL;
            }
            handleKillingMoves(arr[j].cell, &kills[arr[j].cell]);
          } else {
            flag[j] = _FAIL;
          }
        }
        if (flag[j] == _FAIL) {

          directions[j].kill = _NONMOVE;
          directions[j].cell = _NONMOVE;
        }
      }
    }
  }
}

void handleMoves(u8 col, u8 row) {
  int dir = (getCurrPlayer() * 2) - 3;
  u8 c1 = getCell(col - 1, row + dir);
  u8 c2 = getCell(col + 1, row + dir);
  if (c1 == 0)
    reach[col - 1 + (row + dir) * 10] = true;
  if (c2 == 0)
    reach[col + 1 + (row + dir) * 10] = true;
}

void setReach(u8 idx, bool value) { reach[idx] = value; }
