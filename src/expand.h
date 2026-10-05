#ifndef EXPAND_H
#define EXPAND_H
#include "lexer.h"
extern int g_last_status;                 /* value of $? */
int expand_tokens(TokenList *t);          /* 0 ok, -1 error */
#endif
