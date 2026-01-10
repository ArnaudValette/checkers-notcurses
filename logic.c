#include "logic.h"
#include "board.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Max number of different paths in a checkers turn ? */
static bool reach[100] = {};
static Move kills[100] = {};
Rule *rules=NULL;
int n_rules=0;

bool initRules(){
  /* because we want rules to be modified on the fly during the game */
  rules = malloc(128*sizeof(Rule));
  if(rules==NULL) return false;

  /* Basic rules for pawns */
  Rule pawn_kill_front_right = {.kill_location=V2(1,1),.end_location=V2(2,2), .target_cell=TCell_empty, .type=Rule_killing_move_hookable};
  Rule pawn_kill_front_left = {.kill_location=V2(1,-1),.end_location=V2(2,-2), .target_cell=TCell_empty, .type=Rule_killing_move_hookable};
  rules[n_rules++] = pawn_kill_front_right; 
  rules[n_rules++] = pawn_kill_front_left;

  Rule pawn_kill_back_right = {.kill_location=V2(-1,1),.end_location=V2(-2,2), .target_cell=TCell_empty, .type=Rule_killing_move_hookable};
  Rule pawn_kill_back_left = {.kill_location=V2(-1,-1),.end_location=V2(-2,-2), .target_cell=TCell_empty, .type=Rule_killing_move_hookable};
  rules[n_rules++] = pawn_kill_back_left;
  rules[n_rules++] = pawn_kill_back_right;

  Rule pawn_move_front_right = {.kill_location=V2(0,0),.end_location=V2(1,1), .target_cell=TCell_empty, .type=Rule_pacific_move};
  Rule pawn_move_front_left = {.kill_location=V2(0,0),.end_location=V2(1,-1), .target_cell=TCell_empty, .type=Rule_pacific_move};
  rules[n_rules++] = pawn_move_front_left;
  rules[n_rules++] = pawn_move_front_right;

  /*
  Rule pawn_move_front = {.kill_location=V2(0,0),.end_location=V2(1,0), .target_cell=TCell_empty, .type=Rule_pacific_move};
  rules[n_rules++] = pawn_move_front;
  */

  return true;
}

void destroy_rules(){
  if(rules != NULL){
    free(rules);
  }
}

void handle_rules(ui c){

  /* Process Rule_pacific_move first,
   * then Rule_killing_move_hookable:
   * if two rules have the same effect,
   * favor the killing one.
   */

  int player = getCurrPlayer(); // 1 or 2
  int dir = ((player-1) << 1) - 1; // -1 or 1
  ui col =c%10;
  ui row=c/10;

  for(int i = 0; i < n_rules; i++){
    Rule curr = rules[i];
    if(curr.type == Rule_pacific_move){
      V2 target_cell = curr.end_location;
      Target_cell_state target_type = curr.target_cell;
      target_cell.y = (target_cell.y * dir) + row;
      target_cell.x = target_cell.x + col;
      if(target_cell.y < 10  && target_cell.x < 10){
        /* valid move */
        ui scalar_cell = target_cell.y*10 + target_cell.x;
        u8 val = getBoard()[scalar_cell];
        if(target_type == TCell_empty && val == 0){
          reach[scalar_cell] = true;
        } else if(target_type == TCell_occupied && val != 0){
          reach[scalar_cell] = true;
        }
      }
    }
  }

}


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
