#include "c.h"

// macros

// types

// prototypes
extern void outs ARGS((char *));
extern void print ARGS((char *, ...));

// data
// functions
// void outs(char *s) {
//   char *p;
//   for (p = bp; (*p = *s++) != 0; p++) {}
//   bp = p;
//   if (bp > io[fd]->limit) outflush();
// }

void print VARARGS((char *fmt, ...), (fmt, va_alist), va_dcl) {
  va_list ap;

  va_init(ap, fmt);
  vprint(fmt, ap);
  va_end(ap);
}