#include "executor.h"
#include "signals.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

/* TODO (P1 wk2: single command; P2 wk3-4: redirection + pipes; P3: pgid hooks) */
int execute(Pipeline *p) {
    if (!p || p->ncmds == 0) return 0;

    // Phase 2: Basic executor for a single command
    if (p->ncmds == 1) {
        Command *cmd = &p->cmds[0];
        if (!cmd->argv || !cmd->argv[0]) return 0;

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        } else if (pid == 0) {
            // Child
            signals_reset_child();
            execvp(cmd->argv[0], cmd->argv);
            fprintf(stderr, "nsh: %s: command not found\n", cmd->argv[0]);
            exit(127);
        } else {
            // Parent
            int status = 0;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
                return -1;
            }
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                return 128 + WTERMSIG(status);
            }
            return 0;
        }
    }

    fprintf(stderr, "nsh: pipelines not supported yet\n");
    return -1;
}
