#include "trace.h"
#include <stdarg.h>
#include <stdio.h>

int g_trace = 0;

void trace_event(const char *fmt, ...) {
    if (!g_trace) return;
    va_list ap;
    va_start(ap, fmt);
    fputs("[trace] ", stderr);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}
