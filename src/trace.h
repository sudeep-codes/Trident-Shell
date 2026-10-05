#ifndef TRACE_H
#define TRACE_H
extern int g_trace;
void trace_event(const char *fmt, ...);   /* no-op when g_trace == 0 */
#endif
