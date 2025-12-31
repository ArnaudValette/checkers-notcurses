#ifndef INTERACTION_H
#define INTERACTION_H
#include "types.h"
#include <notcurses/notcurses.h>
void blank_state();
bool handle_actions(ncinput *ni, V2 cell_size, V2 dims, char *debug);
#endif
