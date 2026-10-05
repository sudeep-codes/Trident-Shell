#include "signals.h"
#include <signal.h>
#include <stddef.h>

/* TODO (Person 3): sigaction setup, SIGCHLD handler. See docs/SIGNALS_AND_JOBS.md */
void signals_init(void) {
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}

void signals_reset_child(void) {
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
}
