#ifndef BUILTINS_H
#define BUILTINS_H
#include "parser.h"
int is_builtin(const char *name);
int run_builtin(Command *c);    /* returns exit status */
#endif
