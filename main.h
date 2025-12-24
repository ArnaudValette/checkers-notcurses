#ifndef MAIN_H
#define MAIN_H
#include <notcurses/notcurses.h>
#include <stdint.h>

#define iTHREAD(name, attrname,argname, arg) pthread_t name;pthread_attr_t attrname;pthread_attr_init(&attrname);struct input_handler_arg argname= arg;

#define rTHREAD(name, attrname,fun,argname) pthread_create(&name, &attrname, fun, &argname) ;pthread_attr_destroy(&attrname);

int main(int c, char**v);

#endif
