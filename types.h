#ifndef TYPES_H
#define TYPES_H

#define V2(y_,x_) ((V2){.x=(x_),.y=(y_)})
#define v2(y_) ((V2){.x=(y_),.y=(y_)})


#define _NONMOVE 255
#define _PREKILL 254
#include <stdint.h>
typedef unsigned int ui;
typedef uint8_t u8;

typedef struct V2{
  ui y;
  ui x;
} V2;

#endif
