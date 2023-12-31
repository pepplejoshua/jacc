#include <stdarg.h>

// exported macros
#define VARARGS(newlist, oldlist, olddecls) newlist
#define NULL ((void*)0) // machine-independent expression of a nullptr
#define nullptr NULL // ditto

// prototypes and types implemented across files

// alloc.c
// used in the form
// struct T *p; (where T is the shape of the struct)
// p = allocate(sizeof *p, a);
extern void *allocate(unsigned long n, unsigned a); // where n is the number of bytes and a is the arena id
// where a is the arena id
extern void deallocate(unsigned a);
// m is the size of the type in bytes, n is the size of the array, a is the arena id
extern void *new_array(unsigned long m, unsigned long n, unsigned a);
// macros
#define NEW(p, a) ((p) = allocate(sizeof *(p), (a)))
#define NEW0(p, a) memset(NEW((p), (a)), 0, sizeof *(p))
#define mem_align(x) roundup(x, sizeof(union align))


// lex.c
extern Coordinate src;


// list.c
struct list {
  anytype x;
  List link;
};
typedef struct list *List;

// append a List node containing x to list's chain
extern List append(anytype x, List list);
// get the length of the list
extern int len(List list);
// convert a list to a null terminated array allocated in arena a and deallocate it
extern anytype LtoV(List *list, unsigned a);


// output.c
extern void outs(char *);
extern void print(char *, ...);


// string.c
// makes a copy of a null terminated string
extern char *string(char *);
// makes a copy of the len bytes in the string
extern char *stringn(char *, int len);
// takes an integer, converts it to a string and makes a copy of it
extern char *stringd(int);


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
  unsigned temporary : 1;
  unsigned generated : 1;
  unsigned defined : 1;
  Type type; // type of the symbol if any
  float ref; // for some symbols, this estimates the number of times a variable is referenced
  union {
    // specific to labels (p 46)
    struct {
      int label;
      Symbol equatedTo;
    } label_info; // l in lcc
    // struct types (p 65)
    // enum constants (p 69)
    // enum types (p 68)
    // constants (p 47)
    struct {
      Value val;
      Symbol loc; // sym for generated variable, if any
    } constant_info; // c in lcc
    // function symbols (p 290)
    // globals (p 265)
    // temporaries (p 346)
  } additional_info;
  XSymbol x;
  // debugger extensions
};
typedef struct symbol *Symbol;
typedef struct coord {
  char *file; // source file where symbol is located
  unsigned x, y; // y is line and x is column in line where it starts
} Coordinate;
typedef struct table *Table;
typedef union value {
  /* signed */ char sc;
  short ss;
  int i;
  unsigned char uc;
  unsigned short us;
  unsigned int u;
  float f;
  double d;
  void *p;
} Value;
struct table {
  int level;
  Table previous;
  struct entry {
    struct symbol sym;
    struct entry *link;
  } *buckets[256];
  Symbol all;
};
extern Table constants;
extern Table externals; // externed ids
extern Table globals; // file scoped ids
extern Table identifiers; // ordinary ids
extern Table labels; // compiler defined internal labels
extern Table types; // type tags
extern int level;

extern Table new_table(Table, int);
extern void foreach(Table, int, void(*)(Symbol, void *), void *);
extern void enterScope();
extern void exitScope();
extern Symbol install(char *, Table *, int, int);
extern Symbol lookup(char *, Table tp);
extern int genLabel(int);
extern Symbol findLabel(int);
extern Symbol constant(Type, Value);
extern char *vtoa(Type, Value);


// token.h
enum {
#define xx(a,b,c,d,e,f,g) a=b,
#define yy(a,b,c,d,e,f,g)
#include "token.h"
  LAST
};

// types.c
struct type {
  int op;
};
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
enum { PERM = 0, FUNC, STMT }; // which arena to use?
// exported data
// exported functions
void reportNull(void *cand, char *msg) {
  if (cand == NULL) {
    error(msg);
    exit(1);
  }
}