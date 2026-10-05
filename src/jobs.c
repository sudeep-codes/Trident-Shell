#include "jobs.h"

/* TODO (Person 3): job table, state transitions. See docs/SIGNALS_AND_JOBS.md */
Job *job_add(pid_t pgid, pid_t *pids, size_t n, const char *cmd, int bg) {
    (void)pgid; (void)pids; (void)n; (void)cmd; (void)bg;
    return NULL;
}
Job *job_find_by_id(int id) { (void)id; return NULL; }
Job *job_find_by_pgid(pid_t pgid) { (void)pgid; return NULL; }
void job_remove(Job *j) { (void)j; }
void jobs_print(void) {}
int  job_put_fg(Job *j, int cont) { (void)j; (void)cont; return 0; }
int  job_put_bg(Job *j, int cont) { (void)j; (void)cont; return 0; }
