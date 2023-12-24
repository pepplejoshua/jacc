#include "c.h"
// MACROS
#define NEW(p, a) ((p) = allocate(sizeof *(p), (a)))
#define NEW0(p, a) memset(NEW((p), (a)), 0, sizeof *(p))

// TYPES
struct mem_block {
  struct mem_block *next; // next memory block in the arena
  char *limit; // shows the allocatable size of the block
  char *avail; // shows the first free location in the block. Anything before has been used
};
union align {
  long l;
  char *p;
  double d;
  int(*f) ARGS((void));
};
union header {
  struct mem_block b;
  union align a;
};

// debugging implementation using malloc and free
#ifdef PURIFY

#else
// manual implementation

// PROTOTYPES
// used in the form
// struct T *p; (where T is the shape of the struct)
// p = allocate(sizeof *p, a);
extern void *allocate ARGS((unsigned long n, unsigned a)); // where n is the number of bytes and a is the arena id
// where a is the arena id
extern void deallocate ARGS((unsigned a));
// m is the size of the type in bytes, n is the size of the array, a is the arena id
extern void *new_array ARGS((unsigned long m, unsigned long n, unsigned a));

// DATA
static struct mem_block first[] = { { NULL }, { NULL }, { NULL } }, *arena[] = { &first[0], &first[1], &first[2] };
static struct mem_block *free_blocks;

// FUNCTIONS
void *allocate(unsigned long n, unsigned a) {
  struct mem_block *ap = arena[a];
  // round up n to align it properly in memory
  n = roundup(n, sizeof(union align));
  while (ap->avail + n > ap->limit) {
    // get new block from either free blocks or by allocating one
    ap->next = free_blocks;
    if (ap->next != NULL) { // there is at least 1 free mem block
      free_blocks = free_blocks->next; // move the first free block to the next free block
      ap = ap->next;
    } else {
      // allocate new mem block
      unsigned m = sizeof(union header) + n + 10 * 1024; // 10KB
      ap->next = malloc(m); // allocate the memory
      ap = ap->next; // move the ap pointer to the new block
      if (ap == NULL) {
        error("insufficient memmory\n");
        exit(1);
      }
      ap->limit = (char *)ap + m; // set the limit of the block
    }
    ap->avail = (char *)((union header *)ap + 1); // set the avail of the block
    ap->next = NULL; // set the next of the block to NULL since it is the last block
    arena[a] = ap; // set the arena to the new block
  }
  ap->avail += n; // move the avail pointer to the next free location
  return ap->avail - n; // return the location of the allocated memory
}

void deallocate(unsigned a) {
  arena[a]->next = free_blocks; // set the next of the arena to the first free block
  free_blocks = first[a].next; // set the first free block to the arena
  first[a].next = NULL; // set the next of the first block to NULL since it is the last block
  arena[a] = &first[a]; // set the arena to the first block
}

void *new_array(unsigned long m, unsigned long n, unsigned a) {
  return allocate(m * n, a); // allocate m * n bytes of memory
}
#endif