#include "logic.h"
#include "board.h"
#include "rules_provider.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Game § Logic → Initializers & utils                                      ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  
/* Max number of different paths in a checkers turn ? */
static bool reach[100] = {};
static Move kills[100] = {};


void setReach(u8 idx, bool value) { reach[idx] = value; }

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

/* 
╰┭━╾┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅┅╼━┮╮
╭╯ Game § Logic → CORE                                                      ╭╯╿
╙╼━╾┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄━━╪*/
  
void recurse_rules(Rule prev, ui new_col, ui new_row, bool is_king, int player,
                   int dir, int n_rules, Rule *rules) {
  Next_type expecting = prev.next_type;
  for (int i = 0; i < n_rules; i++) {
    Rule curr = rules[i];

    bool is_bind_coherent = (curr.bind == King_bound && is_king) ||
                            (curr.bind == Pawn_bound && !is_king);
    if (!is_bind_coherent) continue;

    bool meets_expectation = false;
    V2 back_cell = prev.end_location;
    back_cell.y = new_row - (back_cell.y * dir);
    back_cell.x = new_col - (back_cell.x * dir);

    V2 target_cell = curr.end_location;
    target_cell.y = (target_cell.y * dir) + new_row;
    target_cell.x = (target_cell.x * dir) + new_col;

    if (target_cell.y >= 10 || target_cell.x >= 10) continue;

    if (curr.type == Rule_killing_move) {

      meets_expectation = (prev.next_type & Next_killing_any_direction) ||
                          ((prev.next_type & Next_killing_same_direction) &&
                           prev.direction == curr.direction);
      if (!meets_expectation) continue;

      V2 killing_cell = curr.kill_location;
      killing_cell.y = (killing_cell.y * dir) + new_row;
      killing_cell.x = (killing_cell.x * dir) + new_row;
      if (killing_cell.y >= 10 || killing_cell.x >= 10) continue;

      ui scalar_kill = (killing_cell.y * 10) + killing_cell.x;
      if (!isOpponentPawnOrKing(scalar_kill)) continue;

      if (prev.type == Rule_killing_move) {
        V2 prev_kill = V2(new_row, new_col);
        prev_kill.y = back_cell.y + (dir * prev.kill_location.y);
        prev_kill.x = back_cell.x + (dir * prev.kill_location.x);

        ui scalar_prev_kill = (prev_kill.y * 10) + prev_kill.x;
      }

    } else if (curr.type == Rule_pacific_move) {
      meets_expectation = (prev.next_type & Next_pacific_any_direction) ||
                          ((prev.next_type & Next_pacific_same_direction) &&
                           prev.direction == curr.direction);
      if (meets_expectation) {
        /* Handling moves (maybe this is a general case and should go out of
         * this scope)*/
      }
    }
  }
}

void handle_rules(ui c) {
  /* Process Rule_pacific_move first,
   * then Rule_killing_move_hookable:
   * if two rules have the same effect,
   * favor the killing one.
   */
  int n_rules = 0;
  Rule *rules = get_basic_rules(&n_rules);

  int player = getCurrPlayer();
  int dir = player == 1 ? 1 : -1;
  ui col = c % 10;
  ui row = c / 10;
  bool king = isKingPawn(col, row);

  for (int i = 0; i < n_rules; i++) {
    Rule curr = rules[i];
    bool is_bind_coherent =
        (king && curr.bind == King_bound) || (!king && curr.bind == Pawn_bound);
    if (is_bind_coherent) {
      if (curr.type == Rule_pacific_move) {
        V2 target_cell = curr.end_location;
        Target_cell_state target_type = curr.target_cell;
        target_cell.y = (target_cell.y * dir) + row;
        target_cell.x = target_cell.x + col;
        if (target_cell.y < 10 && target_cell.x < 10) {
          ui scalar_cell = target_cell.y * 10 + target_cell.x;
          u8 val = getBoard()[scalar_cell];
          bool is_valid_move = false;
          if (target_type == TCell_empty && val == 0) {
            reach[scalar_cell] = true;
            is_valid_move = true;
          } else if (target_type == TCell_occupied && val != 0) {
            reach[scalar_cell] = true;
            is_valid_move = true;
          }
          if (is_valid_move && curr.next_type != Next_null) {
            /* recurse */
          }
        }
      } else if (curr.type == Rule_killing_move) {
      }
    }
  }
}

void handleKillingMoves(u8 c, Move *prev) {
  u8 xi[] = {c - 11, c - 9, c + 11, c + 9};
  u8 ai[] = {c - 22, c - 18, c + 22, c + 18};
  bool flag[] = {0, 0, 0, 0};

  u8 _c = c % 10, _r = c / 10;
  if (_r - 2 >= 0) {
    if (_c - 2 >= 0) flag[0] = 1;
    if (_c + 2 < 10) flag[1] = 1;
  }
  if (_r + 2 < 10) {
    if (_c - 2 >= 0) flag[3] = 1;
    if (_c + 2 < 10) flag[2] = 1;
  }

  for (u8 i = 0; i < 4; i++) {
    if (flag[i]) {
      if (kills[ai[i]].cell == _NONMOVE && ai[i] != getCurrPawn()) {
        if (isOpponentPawnOrKing(getBoard()[xi[i]])) {
          if (getBoard()[ai[i]] == 0) {
            reach[ai[i]] = true;
            kills[ai[i]] = (Move){ai[i], xi[i], NULL};
            if (prev->cell != _NONMOVE) kills[ai[i]].prev = prev;
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
  if (c1 == 0) reach[col - 1 + (row + dir) * 10] = true;
  if (c2 == 0) reach[col + 1 + (row + dir) * 10] = true;
}
