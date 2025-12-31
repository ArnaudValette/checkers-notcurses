#ifndef INPUT_H
#define INPUT_H
#include "types.h"
#include <notcurses/notcurses.h>

typedef struct input_handler_arg {
  struct notcurses *nc;
  V2 cell_size;
  V2 dims;
  char *debug;

} input_handler_arg;

bool ui_changed(ncinput *ni, V2 cell_size, V2 dims, char*debug);
void *handle_input(void *_arg);


#endif
