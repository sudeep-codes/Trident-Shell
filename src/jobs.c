#include "jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define MAX_JOBS 256
static Job g_jobs[MAX_JOBS];
extern int g_last_status;

Job *job_add(pid_t pgid, pid_t *pids, size_t n, const char *cmd, int bg) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (g_jobs[i].id == 0) {
            Job *j = &g_jobs[i];
            j->id = i + 1;
            j->pgid = pgid;
            j->pids = malloc(sizeof(pid_t) * n);
            memcpy(j->pids, pids, sizeof(pid_t) * n);
            j->npids = n;
            j->state = JOB_RUNNING;
            j->cmdline = strdup(cmd);
            j->background = bg;
            return j;
        }
    }
    return NULL;
}
Job *job_find_by_id(int id) {
    for (int i=0; i<MAX_JOBS; i++) {
        if (g_jobs[i].id == id) return &g_jobs[i];
    }
    return NULL;
}
Job *job_find_by_pgid(pid_t pgid) {
    for (int i=0; i<MAX_JOBS; i++) {
        if (g_jobs[i].id != 0 && g_jobs[i].pgid == pgid) return &g_jobs[i];
    }
    return NULL;
}
Job *job_find_by_pid(pid_t pid) {
    for (int i=0; i<MAX_JOBS; i++) {
        if (g_jobs[i].id != 0) {
            for (size_t k=0; k<g_jobs[i].npids; k++) {
                if (g_jobs[i].pids[k] == pid) return &g_jobs[i];
            }
        }
    }
    return NULL;
}
void job_remove(Job *j) {
    if (!j) return;
    free(j->pids);
    free(j->cmdline);
    j->id = 0;
}
void jobs_print(void) {}
int  job_put_fg(Job *j, int cont) {
    if (!j) return 0;
    if (isatty(STDIN_FILENO)) tcsetpgrp(STDIN_FILENO, j->pgid);
    if (cont) {
        kill(-j->pgid, SIGCONT);
    }
    int status;
    waitpid(j->pgid, &status, WUNTRACED);
    if (isatty(STDIN_FILENO)) tcsetpgrp(STDIN_FILENO, getpgrp());
    if (WIFEXITED(status)) {
        j->state = JOB_DONE;
        g_last_status = WEXITSTATUS(status);
        job_remove(j);
    } else if (WIFSIGNALED(status)) {
        j->state = JOB_DONE;
        g_last_status = 128 + WTERMSIG(status);
        fprintf(stderr, "\n");
        job_remove(j);
    } else if (WIFSTOPPED(status)) {
        j->state = JOB_STOPPED;
        j->background = 1;
        fprintf(stderr, "\n[%d] Stopped                 %s\n", j->id, j->cmdline);
    }
    return g_last_status;
}
int  job_put_bg(Job *j, int cont) { (void)j; (void)cont; return 0; }

