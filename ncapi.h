#ifndef NCAPI_H
#define NCAPI_H
#include "types.h"
#include <notcurses/notcurses.h>
#include <stdint.h>

typedef struct {
  uint32_t *buf;
  V2 dim;
} FrameBuffer;

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


typedef struct {
  V2 dim;
  u8 n_cells;
}
gen_board_args;

typedef struct{
  ui radius;
  V2 container_sz;
} gen_pawn_args;


/**
 *  Ready to blit stamp. 
 *
 *  @return A ready to blit data structure (see: #Stamp).
 *  @warning to be freed with #free_stamp
 */
Stamp *stamp(struct ncplane *root, FrameBuffer *buffer,  V2 pos, V2 size_cols_rows);

Stamp *replace_stamp_buffer(Stamp *s, FrameBuffer *buffer);

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
 * stdplane generation shortcut.
 */
struct ncplane *stdplane_util(struct notcurses*nc, unsigned int*y, unsigned int*x, unsigned int *celly, unsigned int*cellx );

#endif
