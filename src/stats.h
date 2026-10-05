#ifndef STATS_H
#define STATS_H
#include <sys/types.h>
#include <sys/resource.h>
void stats_record(pid_t pid, const struct rusage *ru, double wall_sec);
void stats_print(void);
#endif
