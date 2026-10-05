#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
void *xmalloc(size_t n);
void *xrealloc(void *p, size_t n);
char *xstrdup(const char *s);
void  die(const char *msg);          /* perror + exit(1), shell init only */
#endif
