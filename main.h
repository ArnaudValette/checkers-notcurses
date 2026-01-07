#ifndef MAIN_H
#define MAIN_H
#include <notcurses/notcurses.h>
#include <stdint.h>

/** init THREAD variables */
#define iTHREAD(name, attrname) pthread_t name;pthread_attr_t attrname;pthread_attr_init(&attrname);

/** Run THREAD */
#define rTHREAD(name, attrname,fun,argname) pthread_create(&name, &attrname, fun, &argname) ;pthread_attr_destroy(&attrname);

int main(int c, char**v);

#endif
