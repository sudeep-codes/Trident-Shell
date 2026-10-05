#include "builtins.h"
#include <string.h>

static const char *names[] = { "cd", "exit", "export", "jobs", "fg", "bg",
                               "trace", "stats", NULL };

int is_builtin(const char *name) {
    for (int i = 0; names[i]; i++)
        if (strcmp(name, names[i]) == 0) return 1;
    return 0;
}

/* TODO: cd/exit/export (P1), jobs/fg/bg (P3), trace/stats (P1) */
int run_builtin(Command *c) {
    (void)c;
    return 0;
}
