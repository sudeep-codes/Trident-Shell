#include "executor.h"
#include "signals.h"
#include "builtins.h"
#include "jobs.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

extern int g_last_status;

static int apply_redirections(Command *cmd) {
    for (size_t i = 0; i < cmd->nredirs; i++) {
        Redir *r = &cmd->redirs[i];
        int fd;
        if (r->type == REDIR_IN) {
            fd = open(r->path, O_RDONLY);
            if (fd < 0) {
                perror("nsh");
                return -1;
            }
            dup2(fd, 0);
            close(fd);
        } else if (r->type == REDIR_OUT) {
            fd = open(r->path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd < 0) {
                perror("nsh");
                return -1;
            }
            dup2(fd, 1);
            close(fd);
        } else if (r->type == REDIR_APPEND) {
            fd = open(r->path, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (fd < 0) {
                perror("nsh");
                return -1;
            }
            dup2(fd, 1);
            close(fd);
        }
    }
    return 0;
}

int execute(Pipeline *p) {
    if (!p || p->ncmds == 0) return 0;

    if (p->ncmds == 1) {
        Command *cmd = &p->cmds[0];
        if (!cmd->argv || !cmd->argv[0]) return 0;

        if (is_builtin(cmd->argv[0])) {
            int saved_in = dup(0);
            int saved_out = dup(1);
            if (apply_redirections(cmd) < 0) {
                dup2(saved_in, 0); close(saved_in);
                dup2(saved_out, 1); close(saved_out);
                g_last_status = 1;
                return 1;
            }
            int st = run_builtin(cmd);
            g_last_status = st;
            dup2(saved_in, 0); close(saved_in);
            dup2(saved_out, 1); close(saved_out);
            return st;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        } else if (pid == 0) {
            // Child
            signals_reset_child();
            if (p->background) {
                setpgid(0, 0);
            }
            if (apply_redirections(cmd) < 0) {
                exit(1);
            }
            execvp(cmd->argv[0], cmd->argv);
            fprintf(stderr, "nsh: %s: command not found\n", cmd->argv[0]);
            exit(127);
        } else {
            // Parent
            setpgid(pid, pid);
            if (p->background) {
                Job *j = job_add(pid, &pid, 1, p->raw ? p->raw : cmd->argv[0], 1);
                if (j) fprintf(stderr, "[%d] %d\n", j->id, pid);
                return 0;
            } else {
                Job *j = job_add(pid, &pid, 1, p->raw ? p->raw : cmd->argv[0], 0);
                return job_put_fg(j, 0);
            }
        }
    }

    fprintf(stderr, "nsh: pipelines not supported yet\n");
    return -1;
}
