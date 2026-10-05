#include "parser.h"
#include <stdlib.h>

/* TODO (Person 1): recursive-descent parser. See docs/GRAMMAR.md */
int parse(const TokenList *t, Pipeline *out) {
    (void)t;
    out->cmds = NULL; out->ncmds = 0; out->background = 0; out->raw = NULL;
    return -1;
}
void pipeline_free(Pipeline *p) {
    if (!p) return;
    for (size_t i = 0; i < p->ncmds; i++) {
        Command *c = &p->cmds[i];
        if (c->argv) {
            for (char **a = c->argv; *a; a++) free(*a);
            free(c->argv);
        }
        for (size_t r = 0; r < c->nredirs; r++) free(c->redirs[r].path);
        free(c->redirs);
    }
    free(p->cmds);
    free(p->raw);
    p->cmds = NULL; p->ncmds = 0;
}
