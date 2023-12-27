#include <stdarg.h>

// exported macros
#define ARGS(list) list
#define VARARGS(newlist, oldlist, olddecls) newlist
#define NULL ((void*)0) // machine-independent expression of a nullptr
#define nullptr NULL // ditto

// prototypes implemented across files
// alloc.c
// used in the form
// struct T *p; (where T is the shape of the struct)
// p = allocate(sizeof *p, a);
extern void *allocate ARGS((unsigned long n, unsigned a)); // where n is the number of bytes and a is the arena id
// where a is the arena id
extern void deallocate ARGS((unsigned a));
// m is the size of the type in bytes, n is the size of the array, a is the arena id
extern void *new_array ARGS((unsigned long m, unsigned long n, unsigned a));

// list.c
// append a List node containing x to list's chain
extern List append ARGS((anytype x, List list));
// get the length of the list
extern int len ARGS((List list));
// convert a list to a null terminated array allocated in arena a and deallocate it
extern anytype LtoV ARGS((List *list, unsigned a));

// output.c
extern void outs ARGS((char *));
extern void print ARGS((char *, ...));

// string.c
// makes a copy of a null terminated string
extern char *string ARGS((char *));
// makes a copy of the len bytes in the string
extern char *stringn ARGS((char *, int len));
// takes an integer, converts it to a string and makes a copy of it
extern char *stringd ARGS((int));

// returns the length of an array by dividing the overall size by the size of one elem
#define NELEMS(a) ((int) (sizeof (a) / sizeof ((a)[0])))
// `((x) + ((n) - 1))` adds `n - 1` to `x`.
// This ensures that if `x` is not already a multiple of `n`, it will be increased to the next multiple of `n`.
// If `x` is already a multiple of `n`, it will not be changed.
// The expression `&(~((n)-1))` performs a bitwise AND operation with the bitwise complement of `n - 1`.
// This effectively rounds down `x + n - 1` to the nearest multiple of `n`.
// The reason this works is that for any power of 2, `n - 1` has all lower bits set to 1 and all higher bits set to 0.
// The bitwise complement of `n - 1` has all lower bits set to 0 and all higher bits set to 1.
// So, the bitwise AND operation with `x + n - 1` clears all the lower bits of `x + n - 1`,
// effectively rounding it down to the nearest multiple of `n`.
#define roundup(x, n) (((x) + ((n) - 1))&(~((n) - 1))) // round x to the next multiple of n (which must be a power of 2)
#define va_init(a, b) va_start(a, b)

// typedefs
typedef void *anytype;

#include "config.h"

// interfaces (page 78)
// exported types
enum { PERM = 0, FUNC, STMT };
// exported data
// exported functions
void reportNull(void *cand, char *msg) {
  if (cand == NULL) {
    error(msg);
    exit(1);
  }
}
