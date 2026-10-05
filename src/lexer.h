#ifndef LEXER_H
#define LEXER_H
#include <stddef.h>
typedef enum { TOK_WORD, TOK_PIPE, TOK_REDIR_IN, TOK_REDIR_OUT,
               TOK_REDIR_APPEND, TOK_BG, TOK_EOF } TokenType;
typedef struct { TokenType type; char *text; } Token;
typedef struct { Token *items; size_t len; } TokenList;
int  lex(const char *line, TokenList *out);  /* 0 ok, -1 syntax error */
void tokenlist_free(TokenList *t);
#endif
