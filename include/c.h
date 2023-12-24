#include <stdarg.h>

// exported macros
#define ARGS(list) list
#define VARARGS(newlist, oldlist, olddecls) newlist
#define NULL ((void*)0) // machine-independent expression of a nullptr
#define nullptr NULL // ditto

#define len(a) ((int) (sizeof (a) / sizeof ((a)[0]))) // returns the length of an array by dividing the overall size by the size of one elem
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

#include "config.h"

// interfaces (page 78)
// exported types
// exported data
// exported functions
