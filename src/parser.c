#include "parser.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void free_command_partial(Command *c) {
    if (c->argv) {
        for (char **a = c->argv; *a; a++) free(*a);
        free(c->argv);
    }
    if (c->redirs) {
        for (size_t r = 0; r < c->nredirs; r++) free(c->redirs[r].path);
        free(c->redirs);
    }
}

int parse(const TokenList *t, Pipeline *out) {
    out->cmds = NULL; out->ncmds = 0; out->background = 0; out->raw = NULL;
    
    if (!t || t->len == 0 || t->items[0].type == TOK_EOF) return 0;

    size_t i = 0;
    while (i < t->len && t->items[i].type != TOK_EOF) {
        if (t->items[i].type == TOK_BG) {
            out->background = 1;
            i++;
            break;
        }

        Command cmd = {NULL, NULL, 0};
        size_t argv_cap = 0, redir_cap = 0, argc = 0;
        int got_word = 0;

        while (i < t->len && t->items[i].type != TOK_EOF && t->items[i].type != TOK_PIPE && t->items[i].type != TOK_BG) {
            Token *tok = &t->items[i];
            if (tok->type == TOK_WORD) {
                if (argc >= argv_cap) {
                    argv_cap = argv_cap ? argv_cap * 2 : 8;
                    cmd.argv = xrealloc(cmd.argv, (argv_cap + 1) * sizeof(char*));
                }
                cmd.argv[argc++] = xstrdup(tok->text);
                cmd.argv[argc] = NULL;
                got_word = 1;
                i++;
            } else if (tok->type == TOK_REDIR_IN || tok->type == TOK_REDIR_OUT || tok->type == TOK_REDIR_APPEND) {
                TokenType rt = tok->type;
                i++;
                if (i >= t->len || t->items[i].type != TOK_WORD) {
                    fprintf(stderr, "nsh: syntax error near redirection\n");
                    free_command_partial(&cmd);
                    goto error;
                }
                if (cmd.nredirs >= redir_cap) {
                    redir_cap = redir_cap ? redir_cap * 2 : 4;
                    cmd.redirs = xrealloc(cmd.redirs, redir_cap * sizeof(Redir));
                }
                Redir *r = &cmd.redirs[cmd.nredirs++];
                r->type = (rt == TOK_REDIR_IN) ? REDIR_IN : ((rt == TOK_REDIR_OUT) ? REDIR_OUT : REDIR_APPEND);
                r->path = xstrdup(t->items[i].text);
                i++;
            } else {
                fprintf(stderr, "nsh: syntax error\n");
                free_command_partial(&cmd);
                goto error;
            }
        }

        if (!got_word && cmd.nredirs == 0) {
            fprintf(stderr, "nsh: syntax error near '%s'\n", (i < t->len && t->items[i].type == TOK_PIPE) ? "|" : "newline");
            free_command_partial(&cmd);
            goto error;
        }

        if (!cmd.argv) {
            cmd.argv = xmalloc(sizeof(char*));
            cmd.argv[0] = NULL;
        }

        out->cmds = xrealloc(out->cmds, (out->ncmds + 1) * sizeof(Command));
        out->cmds[out->ncmds++] = cmd;

        if (i < t->len && t->items[i].type == TOK_PIPE) {
            i++;
            if (i >= t->len || t->items[i].type == TOK_EOF || t->items[i].type == TOK_PIPE || t->items[i].type == TOK_BG) {
                fprintf(stderr, "nsh: syntax error near '|'\n");
                goto error;
            }
        }
    }

    if (i < t->len && t->items[i].type != TOK_EOF) {
        fprintf(stderr, "nsh: syntax error near unexpected token\n");
        goto error;
    }

    return 0;

error:
    pipeline_free(out);
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
