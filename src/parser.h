#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>
#include "lexer.h"
typedef enum { REDIR_IN, REDIR_OUT, REDIR_APPEND } RedirType;
typedef struct { RedirType type; char *path; } Redir;
typedef struct {
    char  **argv;                 /* NULL-terminated */
    Redir  *redirs; size_t nredirs;
} Command;
typedef struct {
    Command *cmds; size_t ncmds;
    int background;
    char *raw;
} Pipeline;
int  parse(const TokenList *t, Pipeline *out);   /* 0 ok, -1 error */
void pipeline_free(Pipeline *p);
#endif
