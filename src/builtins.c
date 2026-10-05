#include "builtins.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int g_last_status;

static const char *names[] = { "cd", "exit", "export", "jobs", "fg", "bg",
                               "trace", "stats", NULL };

int is_builtin(const char *name) {
    if (!name) return 0;
    for (int i = 0; names[i]; i++)
        if (strcmp(name, names[i]) == 0) return 1;
    return 0;
}

int run_builtin(Command *c) {
    if (!c || !c->argv || !c->argv[0]) return 0;
    const char *name = c->argv[0];
    
    if (strcmp(name, "cd") == 0) {
        const char *dir = c->argv[1];
        if (!dir) {
            dir = getenv("HOME");
            if (!dir) {
                fprintf(stderr, "nsh: cd: HOME not set\n");
                return 1;
            }
        }
        if (chdir(dir) != 0) {
            perror("nsh: cd");
            return 1;
        }
        return 0;
    } else if (strcmp(name, "exit") == 0) {
        if (c->argv[1]) exit(atoi(c->argv[1]));
        exit(g_last_status);
    } else if (strcmp(name, "export") == 0) {
        if (c->argv[1]) {
            char *eq = strchr(c->argv[1], '=');
            if (eq) {
                *eq = '\0';
                setenv(c->argv[1], eq + 1, 1);
                *eq = '=';
            } else {
                setenv(c->argv[1], "", 1);
            }
        }
        return 0;
    }
    return 0;
}
