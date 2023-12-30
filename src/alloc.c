#include "c.h"

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
  int(*f)(void);
};
union header {
  struct mem_block b;
  union align a;
};

// PROTOTYPES

// debugging implementation using malloc and free
#ifdef PURIFY
union header *arenas[3];

void *allocate(unsigned long n, unsigned a) {
  assert(a < NELEMS(arenas));
  union header *n_header = malloc(sizeof * n_header + n); // allocate space for header and n bytes
  reportNull(n_header, "insufficient memory\n");
  n_header->b.next = (void *)arenas[a]; // point to current start of arena
  arenas[a] = n_header; // make new block start of arena
  return n_header + 1; // increment to the start of the memory
}

void deallocate(unsigned a) {
  // 2 blocks working in tandem to delete all the blocks
  // in an arena
  union header *p, *q;

  assert(a < NELEMS(arenas));
  // start at the most recently allocated block till p is NULL
  // and update p with the value of q
  for (p = arenas[a]; p; p = q) {
    q = (void *)p->b.next; // get the next pointer for p and set it to q (so we can process it next)
    free(p); // delete p block
  }
  arenas[a] = NULL; // reset state of arena
}
#else
// DATA
static struct mem_block first[] = { { NULL }, { NULL }, { NULL } }, *arenas[] = { &first[0], &first[1], &first[2] };
static struct mem_block *free_blocks;

// FUNCTIONS

void *allocate(unsigned long n, unsigned a) {
  assert(a < NELEMS(arenas));
  assert(n > 0);
  struct mem_block *ap = arenas[a];
  // round up n to align it properly in memory
  n = mem_align(n);
  while (ap->avail + n > ap->limit) {
    // get new block from either free blocks or by allocating one
    ap->next = free_blocks;
    if (ap->next != NULL) { // there is at least 1 free mem block
      free_blocks = free_blocks->next; // move the first free block to the next free block
      ap = ap->next; // move the ap pointer to the new block
    } else {
      // allocate new mem block
      // sizeof(union header) is the size of the header of the block
      // the header stores metadata about the block like the size of the block,
      // the next block, etc.
      unsigned m = sizeof(union header) + n + mem_align(10 * 1024); // 24 bytes (on my 64-bit computer) + n bytes + at least 10240 bytes
      ap->next = malloc(m); // allocate the memory
      ap = ap->next; // move the ap pointer to the new block
      reportNull(ap, "insufficient memory\n");
      ap->limit = (char *)ap + m; // set the limit of the block to starting addr + size of block
    }
    // set the avail of the block to the point right after the header of the block
    // the header is of size sizeof(union header)
    ap->avail = (char *)((union header *)ap + 1);
    ap->next = NULL; // set the next of the block to NULL since it is the last block
    arenas[a] = ap; // set the arena to the new block
  }
  ap->avail += n; // move the avail pointer to the next free location
  return ap->avail - n; // return the location of the allocated memory
}

void deallocate(unsigned a) {
  while (arenas[a]->next != NULL) { // while there are more blocks in the arena
    arenas[a] = arenas[a]->next; // move the arena pointer to the next block
  }
  // set the next pointer of the last block to the first free block
  arenas[a]->next = free_blocks;
  // let the free blocks now start from the first block of this arena, effectively
  // freeing all the blocks in the arena for reuse
  free_blocks = first[a].next;
  // set the next of the first block to NULL since it is the last block
  first[a].next = NULL;
  // set the arena to the first block
  arenas[a] = &first[a];
}
#endif

void *new_array(unsigned long m, unsigned long n, unsigned a) {
  return allocate(m * n, a); // allocate m * n bytes of memory
}