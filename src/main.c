#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "expand.h"
#include "parser.h"
#include "executor.h"
#include "signals.h"
#include <unistd.h>

/* REPL skeleton. Currently every line is a "not implemented" stub. */
int main(void) {
    char *line = NULL;
    size_t cap = 0;

    signals_init();
    int interactive = isatty(STDIN_FILENO);
    for (;;) {
        if (interactive) {
            fputs("nsh> ", stdout);
            fflush(stdout);
        }
        if (getline(&line, &cap, stdin) < 0) {
            if (interactive) putchar('\n');
            break;
        }
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;

        TokenList toks;
        Pipeline  pl;
        if (lex(line, &toks) < 0) { continue; }
        if (expand_tokens(&toks) == 0 && parse(&toks, &pl) == 0) {
            g_last_status = execute(&pl);
            pipeline_free(&pl);
        }
        tokenlist_free(&toks);
    }
    free(line);
    return 0;
}
