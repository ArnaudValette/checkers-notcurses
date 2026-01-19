#ifndef RULES_PROVIDER_H
#define RULES_PROVIDER_H
#include "types.h"
#include <stdbool.h>

typedef enum {MD_F, MD_FR, MD_R, MD_BR, MD_B, MD_BL, MD_L, MD_FL} Move_direction;

typedef enum {TCell_empty, TCell_occupied, TCell_killed} Target_cell_state;

typedef enum {Rule_killing_move, Rule_pacific_move} Rule_type;

typedef enum {
  Next_null = 0,
  Next_pacific_same_direction=1<<0,
  Next_killing_same_direction=1<<1,
  Next_any_same_direction=1<<0|1<<1,
  Next_pacific_any_direction=1<<2|1<<0,
  Next_killing_any_direction=1<<3|1<<1,
  Next_any_any_direction=1<<0|1<<1|1<<2|1<<3,
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

/* Specification for a datastructure:
   we need to:
   a) know all cells of the board if they are reachable by a given pawn 
   b) know what is the highest kill count, shortest path to a cell from a pawn

   a) is solved by exploring BFS/DFS. Our exploration takes the form of a tree of possible moves, a branch of the tree should stop growing if it circles or if it rejoin a previously built branch.
   So:
   - a branch cannot reach the same cell twice (= no chains)
   - a branch cannot reach a cell another branch reached with the same "context" [(position, next_type), which are stored for each node of each branch]
   - We perform a BFS until all branches are stopped, allowing us to know which cells are reachable in one go

   b) is solved by building a tree and storing for each node of each branch (position, next_type, kill_count, length) we want to find a solution that minimize length and maximize kill_count.

   We need a convenient datastructure that allows us to store "contexts" in a dynamic fashion.
   Also, given a specific position, we should be able to know the context of different branches, to know if we differ.
   We need to implement "domination" i.e. branches that reach the same position with the same "next_type" value should be compared (in their kill_count and then length value) to keep only the one(s) that have the highest kc and the least length value. 
   We should be able to remove branches and contexts on the go.

   A hashmap could be used to store contexts : (position, next_type)->[&nodeA,&nodeB] (dynamic array)

   an array of branch roots (given a branch is a linked list with child:[&node, &node] a dynamic array)

   We need to implement : 
   - dynamic arrays
   - hashmap
   - Branching
 */

typedef struct context{
  ui position;
  Rule *rule;
  ui nkill;
  ui plength;
} context;

bool init_basic_rules();
Rule* get_basic_rules(int*);
void destroy_rules();

#endif 
