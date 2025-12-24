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

int main(int c, char**v);

#endif
