#include "rules_provider.h"
#include <stdlib.h>

Rule *rules = NULL;
int n_rules = 0;

bool init_basic_rules() {
  /* because we want rules to be modified on the fly during the game */
  rules = malloc(128 * sizeof(Rule));
  n_rules = 0;
  if (rules == NULL) return false;

  /* Basic rules for pawns */
  Rule pawn_kill_front_right = {.kill_location = V2(1, 1),
                                .end_location = V2(2, 2),
                                .target_cell = TCell_empty,
                                .type = Rule_killing_move_hookable,
                                .next_type = Next_killing,
                                .direction = MD_FR,
                                .bind = Pawn_bound};
  Rule pawn_kill_front_left = {.kill_location = V2(1, -1),
                               .end_location = V2(2, -2),
                               .target_cell = TCell_empty,
                               .type = Rule_killing_move_hookable,
                               .next_type = Next_killing,
                               .direction = MD_FL,
                               .bind = Pawn_bound};
  rules[n_rules++] = pawn_kill_front_right;
  rules[n_rules++] = pawn_kill_front_left;

  Rule pawn_kill_back_right = {.kill_location = V2(-1, 1),
                               .end_location = V2(-2, 2),
                               .target_cell = TCell_empty,
                               .type = Rule_killing_move_hookable,
                               .next_type = Next_killing,
                               .direction = MD_BR,
                               .bind = Pawn_bound};
  Rule pawn_kill_back_left = {.kill_location = V2(-1, -1),
                              .end_location = V2(-2, -2),
                              .target_cell = TCell_empty,
                              .type = Rule_killing_move_hookable,
                              .next_type = Next_killing,
                              .direction = MD_BL,
                              .bind = Pawn_bound};
  rules[n_rules++] = pawn_kill_back_left;
  rules[n_rules++] = pawn_kill_back_right;

  Rule pawn_move_front_right = {.kill_location = V2(0, 0),
                                .end_location = V2(1, 1),
                                .target_cell = TCell_empty,
                                .type = Rule_pacific_move,
                                .next_type = Next_null,
                                .direction = MD_FR,
                                .bind = Pawn_bound};
  Rule pawn_move_front_left = {.kill_location = V2(0, 0),
                               .end_location = V2(1, -1),
                               .target_cell = TCell_empty,
                               .type = Rule_pacific_move,
                               .next_type = Next_null,
                               .direction = MD_FL,
                               .bind = Pawn_bound};
  rules[n_rules++] = pawn_move_front_left;
  rules[n_rules++] = pawn_move_front_right;

  /*
  Rule pawn_move_front = {.kill_location = V2(0, 0),
                          .end_location = V2(1, 0),
                          .target_cell = TCell_empty,
                          .type = Rule_pacific_move,
                          .next_type = Next_null,
                          .direction = MD_F,
                          .bind = Pawn_bound};

  rules[n_rules++] = pawn_move_front;
  */

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
