#ifndef RULES_PROVIDER_H
#define RULES_PROVIDER_H
#include "types.h"
#include <stdbool.h>

typedef enum {MD_F, MD_FR, MD_R, MD_BR, MD_B, MD_BL, MD_L, MD_FL} Move_direction;
typedef enum {TCell_empty, TCell_occupied, TCell_killed} Target_cell_state;

typedef enum {Rule_killing_move, Rule_pacific_move} Rule_type;

typedef enum {
  Next_null = 0,
  Next_pacific_any_direction=1<<0,
  Next_killing_any_direction=1<<1,
  Next_pacific_same_direction=1<<2,
  Next_killing_same_direction=1<<3,
} Next_type;
typedef enum {King_bound, Pawn_bound} Bind_type;

typedef struct rule{
  V2 end_location;
  V2 kill_location;
  Target_cell_state target_cell;
  Move_direction direction;
  Rule_type type;
  Next_type next_type;
  Bind_type bind;
} Rule;

bool init_basic_rules();
Rule* get_basic_rules(int*);
void destroy_rules();

#endif 
