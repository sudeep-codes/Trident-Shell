#include "lexer.h"
#include <stdlib.h>

/* TODO (Person 1): tokenizer with quotes/escapes/operators. See docs/GRAMMAR.md */
int lex(const char *line, TokenList *out) {
    (void)line;
    out->items = NULL;
    out->len = 0;
    return -1;
}
void tokenlist_free(TokenList *t) {
    if (!t) return;
    for (size_t i = 0; i < t->len; i++) free(t->items[i].text);
    free(t->items);
    t->items = NULL;
    t->len = 0;
}
