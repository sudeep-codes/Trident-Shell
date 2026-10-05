#include "stats.h"

/* TODO (Person 1): store rusage per pid/job, print table */
void stats_record(pid_t pid, const struct rusage *ru, double wall_sec) {
    (void)pid; (void)ru; (void)wall_sec;
}
void stats_print(void) {}
