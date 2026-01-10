#ifndef LOGIC_H
#define LOGIC_H
#include "types.h"
#include <stdbool.h>

#define kingMove(__col, __row)                                                 \
  (KingHelper) { (__col) + (__row) * 10, getCell((__col), (__row)) }

typedef enum {TCell_empty, TCell_occupied, TCell_killed} Target_cell_state;
typedef enum {Rule_killing_move_hookable, Rule_pacific_move} Rule_type;
/* no king for now */
//typedef enum {King_bound, Pawn_bound} Bind_type;

typedef struct rule{
  V2 end_location;
  V2 kill_location;
  Target_cell_state target_cell;
  Rule_type type;
  //Bind_type bind;
} Rule;

typedef struct move {
  u8 cell;
  u8 kill;
  struct move *prev;
} Move;

typedef struct {
  u8 cell;
  int value;
} KingHelper;

typedef enum { _FREE = 0, _ENEMY, _FAIL, _SUCCESS } KingMoveStateFlag;

bool initRules();
void destroy_rules();

void handle_rules(ui c);

void setReach(u8 idx, bool value);
void resetReach();
void resetKills();
bool isReachable(u8 col, u8 row);
bool isKillingMoveOption(u8 col, u8 row);
void handleKillingMoves(u8 c, Move *prev);
void handleKingKillingMoves(u8 c);
void handleMoves(u8 col, u8 row);
Move *getKillBuffer();

#endif
