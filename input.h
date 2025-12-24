#ifndef INPUT_H
#define INPUT_H
#include <notcurses/notcurses.h>

typedef struct input_handler_arg {
  struct notcurses *nc;

} input_handler_arg;



bool ui_changed(ncinput *ni);
void *handle_input(void *_arg);


#endif
