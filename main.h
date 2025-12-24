#ifndef MAIN_H
#define MAIN_H
#include <notcurses/notcurses.h>
#include <stdint.h>

/**
 * @brief Abort execution if condition is true
 * 
 * @param cond Condition to test
 * @param exp Expression executed before abort
 * @warning A ret: label should exist !
 */
#define _CHECK(cond, exp) do{ if(cond){ exp; goto ret;}} while(0);

#define V2(y_,x_) ((V2){.x=(x_),.y=(y_)})
#define v2(y_) ((V2){.x=(y_),.y=(y_)})

typedef unsigned int ui;
typedef uint8_t u8;

typedef struct V2{
  ui y;
  ui x;
} V2;

int main(int c, char**v);

/**
 *  #Stamp, a convenient structure that links a ncplane, an ncvisual,
 *  and their opts.
 *  Useful when your needs are basic.
 */
typedef struct stamp{
  struct ncplane *plane;
  struct ncvisual *visual;
  struct ncplane_options popts;
  struct ncvisual_options vopts;
} Stamp;

/**
 *  Ready to blit stamp. "f" handles the hard part, this is a shortcut (see: #generate_board)
 *
 *  @return A ready to blit data structure (see: #Stamp).
 *  @warning to be freed with #free_stamp
 */
Stamp *stamp(struct ncplane *root, struct ncvisual *(*f)(V2), V2 pos, V2 spx, V2 sz);

/**
 * Blit shortcut.
 */
void blit_stamp(struct notcurses *nc, Stamp *s);

/**
 * Cleanly dispose #Stamps
 */
void free_stamp(Stamp *s);

/**
 * #notcurses initialization shortcut
 */
struct notcurses *init();

/**
 * Example implementation of a #stamp f parameter.
 * (uses #ncvisual_from_rgba to create a 8x8 checker board)
 */
struct ncvisual *generate_board(V2 dim);

/**
 * stdplane generation shortcut.
 */
struct ncplane *stdplane_util(struct notcurses*nc, unsigned int*y, unsigned int*x, unsigned int *celly, unsigned int*cellx );

#endif
