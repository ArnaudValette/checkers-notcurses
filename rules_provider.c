#include "rules_provider.h"
#include <stdlib.h>

Rule *rules = NULL;
int n_rules = 0;

bool init_basic_rules() {
  /* because we want rules to be modified on the fly during the game */
  rules = malloc(128 * sizeof(Rule));
  n_rules = 0;
  if (rules == NULL) return false;

  /* Basic rules for pawns and kings encoded as if white played them.
     - flying kings
   */

  Rule pawn_kill_fr = {.kill_location = V2(-1, 1),
                       .end_location = V2(-2, 2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_FR,
                       .bind = Pawn_bound};
  Rule pawn_kill_fl = {.kill_location = V2(-1, -1),
                       .end_location = V2(-2, -2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_FL,
                       .bind = Pawn_bound};
  rules[n_rules++] = pawn_kill_fr;
  rules[n_rules++] = pawn_kill_fl;

  Rule pawn_kill_br = {.kill_location = V2(1, 1),
                       .end_location = V2(2, 2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_BR,
                       .bind = Pawn_bound};
  Rule pawn_kill_bl = {.kill_location = V2(1, -1),
                       .end_location = V2(2, -2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_BL,
                       .bind = Pawn_bound};
  rules[n_rules++] = pawn_kill_bl;
  rules[n_rules++] = pawn_kill_br;

  Rule pawn_move_fr = {.kill_location = V2(0, 0),
                       .end_location = V2(-1, 1),
                       .target_cell = TCell_empty,
                       .type = Rule_pacific_move,
                       .next_type = Next_null,
                       .direction = MD_FR,
                       .bind = Pawn_bound};
  Rule pawn_move_fl = {.kill_location = V2(0, 0),
                       .end_location = V2(-1, -1),
                       .target_cell = TCell_empty,
                       .type = Rule_pacific_move,
                       .next_type = Next_null,
                       .direction = MD_FL,
                       .bind = Pawn_bound};
  rules[n_rules++] = pawn_move_fl;
  rules[n_rules++] = pawn_move_fr;

  Rule king_move_ray_fl = {.kill_location = v2(0),
                           .end_location = V2(-1, -1),
                           .target_cell = TCell_empty,
                           .type = Rule_pacific_move,
                           .next_type = Next_pacific_same_direction |
                                        Next_killing_same_direction,
                           .direction = MD_FL,
                           .bind = King_bound};
  Rule king_move_ray_fr = {.kill_location = v2(0),
                           .end_location = V2(-1, 1),
                           .target_cell = TCell_empty,
                           .type = Rule_pacific_move,
                           .next_type = Next_pacific_same_direction |
                                        Next_killing_same_direction,
                           .direction = MD_FR,
                           .bind = King_bound};
  Rule king_move_ray_bl = {.kill_location = v2(0),
                           .end_location = V2(1, -1),
                           .target_cell = TCell_empty,
                           .type = Rule_pacific_move,
                           .next_type = Next_pacific_same_direction |
                                        Next_killing_same_direction,
                           .direction = MD_BL,
                           .bind = King_bound};
  Rule king_move_ray_br = {.kill_location = v2(0),
                           .end_location = V2(1, 1),
                           .target_cell = TCell_empty,
                           .type = Rule_pacific_move,
                           .next_type = Next_pacific_same_direction |
                                        Next_killing_same_direction,
                           .direction = MD_BR,
                           .bind = King_bound};
  rules[n_rules++] = king_move_ray_fr;
  rules[n_rules++] = king_move_ray_fl;
  rules[n_rules++] = king_move_ray_bl;
  rules[n_rules++] = king_move_ray_br;

  Rule king_kill_fl = {.kill_location = V2(-1, -1),
                       .end_location = V2(-2, -2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_FL,
                       .bind = King_bound};
  Rule king_kill_fr = {.kill_location = V2(-1, 1),
                       .end_location = V2(-2, 2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_FR,
                       .bind = King_bound};
  Rule king_kill_bl = {.kill_location = V2(1, -1),
                       .end_location = V2(2, -2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_BL,
                       .bind = King_bound};
  Rule king_kill_br = {.kill_location = V2(1, 1),
                       .end_location = V2(2, 2),
                       .target_cell = TCell_empty,
                       .type = Rule_killing_move,
                       .next_type = Next_killing_any_direction,
                       .direction = MD_BR,
                       .bind = King_bound};
  rules[n_rules++] = king_kill_fr;
  rules[n_rules++] = king_kill_fl;
  rules[n_rules++] = king_kill_br;
  rules[n_rules++] = king_kill_bl;

  return true;
}

Rule *get_basic_rules(int *n) {
  if (rules == NULL || n_rules == 0) {
    init_basic_rules();
  }
  *n = n_rules;
  return rules;
}

void destroy_rules() {
  if (rules != NULL) {
    free(rules);
    rules = NULL;
  }
  n_rules = 0;
}
