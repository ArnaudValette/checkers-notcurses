#ifndef RULES_PROVIDER_H
#define RULES_PROVIDER_H
#include "types.h"
#include <stdbool.h>

typedef enum {MD_F, MD_FR, MD_R, MD_BR, MD_B, MD_BL, MD_L, MD_FL} Move_direction;
typedef enum {TCell_empty, TCell_occupied, TCell_killed} Target_cell_state;
typedef enum {Rule_killing_move_hookable, Rule_pacific_move} Rule_type;
typedef enum {Next_pacific, Next_null, Next_any, Next_any_same_direction, Next_killing} Next_type;
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
