#include "lexer.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int lex(const char *line, TokenList *out) {
    out->items = NULL;
    out->len = 0;
    size_t cap = 0;

    const char *p = line;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p || *p == '\n') break;
        if (*p == '#') break; // comment

        if (*p == '|') {
            if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
            out->items[out->len++] = (Token){TOK_PIPE, NULL};
            p++; continue;
        }
        if (*p == '&') {
            if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
            out->items[out->len++] = (Token){TOK_BG, NULL};
            p++; continue;
        }
        if (*p == '<') {
            if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
            out->items[out->len++] = (Token){TOK_REDIR_IN, NULL};
            p++; continue;
        }
        if (*p == '>') {
            if (p[1] == '>') {
                if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
                out->items[out->len++] = (Token){TOK_REDIR_APPEND, NULL};
                p += 2; continue;
            } else {
                if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
                out->items[out->len++] = (Token){TOK_REDIR_OUT, NULL};
                p++; continue;
            }
        }

        const char *start = p;
        int state = 0; // 0=normal, 1=sq, 2=dq, 3=esc_norm, 4=esc_dq
        while (*p) {
            if (state == 0) {
                if (*p == ' ' || *p == '\t' || *p == '\n') break;
                if (*p == '|' || *p == '&' || *p == '<' || *p == '>') break;
                if (*p == '\'') state = 1;
                else if (*p == '"') state = 2;
                else if (*p == '\\') state = 3;
            } else if (state == 1) {
                if (*p == '\'') state = 0;
            } else if (state == 2) {
                if (*p == '"') state = 0;
                else if (*p == '\\') state = 4;
            } else if (state == 3) {
                state = 0;
            } else if (state == 4) {
                state = 2;
            }
            p++;
        }

        if (state == 1 || state == 2 || state == 4) {
            fprintf(stderr, "nsh: unterminated quote\n");
            tokenlist_free(out);
            return -1;
        }

        size_t len = p - start;
        char *text = xmalloc(len + 1);
        memcpy(text, start, len);
        text[len] = '\0';
        if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
        out->items[out->len++] = (Token){TOK_WORD, text};
    }
    if (out->len >= cap) { cap = cap ? cap * 2 : 16; out->items = xrealloc(out->items, cap * sizeof(Token)); }
    out->items[out->len++] = (Token){TOK_EOF, NULL};
    return 0;
}

void tokenlist_free(TokenList *t) {
    if (!t) return;
    for (size_t i = 0; i < t->len; i++) free(t->items[i].text);
    free(t->items);
    t->items = NULL;
    t->len = 0;
}
