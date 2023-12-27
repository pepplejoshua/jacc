#include <stdarg.h>

// exported macros
#define ARGS(list) list
#define VARARGS(newlist, oldlist, olddecls) newlist
#define NULL ((void*)0) // machine-independent expression of a nullptr
#define nullptr NULL // ditto

// prototypes and types implemented across files
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
struct list {
  anytype x;
  List link;
};
typedef struct list *List;

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

// sym.c
enum { CONSTANTS = 1, LABELS, GLOBAL, PARAM, LOCAL }; // SCOPE of symbol
struct symbol {
  char *name; // usually sym table key. holds name from source or generated numerical string
  int scope; // is one of the scopes above
  Coordinate src; // location of symbol in source file
  Symbol up; // links to the symbols available in scope before it in the sym table
  List uses; // keeps track of all the uses of this sym through a list of Coordinates
  int sclass; // keeps track of a symbols extended storage class (AUTO | REGISTER | STATIC | EXTERN)
  // symbol flags (p 50)
  Type type; // type of the symbol if any
  float ref; // for some symbols, this estimates the number of times a variable is referenced
  union {
    // labels (p 46)
    // struct types (p 65)
    // enum constants (p 69)
    // enum types (p 68)
    // constants (p 47)
    // function symbols (p 290)
    // globals (p 265)
    // temporaries (p 346)
  } u;
  XSymbol x;
  // debugger extensions
};
typedef struct symbol *Symbol;
typedef struct coord {
  char *file; // source file where symbol is located
  unsigned x, y; // y is line and x is column in line where it starts
} Coordinate;
typedef struct table *Table;
struct table {
  int level;
  Table previous;
  struct entry {
    struct symbol sym;
    struct entry *link;
  } *buckets[256];
  Symbol all;
};

// types.c
struct type {};
typedef struct type *Type;

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
