#include "c.h"

// MACROS

// TYPES
struct list {
  anytype x;
  List link;
};
typedef struct list *List;

// DATA
static List free_nodes;

// FUNCTIONS
List append(anytype x, List list) {
  List new_l = free_nodes;

  if (new_l != NULL) {
    free_nodes = free_nodes->link;
  } else {
    NEW(new_l, PERM);
  }

  if (list) {
    new_l->link = list->link;
    list->link = new_l;
  } else {
    new_l->link = new_l;
  }

  new_l->x = x;
  return new_l;
}

int len(List list) {
  int n = 0;

  if (list) {
    List p = list;
    do {
      n++;
    } while ((p = p->link) != list);
  }

  return n;
}

anytype LtoV(List *list, unsigned arena) {
  int i = 0;
  // an array of void * elements
  void **array = new_array(len(list) + 1, sizeof array[0], arena);

  if (*list) { // derefs into a List or list *
    List lp = *list;
    do {
      lp = lp->link; // move to next node, since list is the last node in the link
      array[i] = lp->x;
      i++;
    } while (lp != *list);
#ifndef PURIFY
    lp = (*list)->link; // store the start of the list
    (*list)->link = free_nodes; // point end of list to start of free nodes
    free_nodes = lp; // free node will now point to the start of list
#endif
  }

  *list = NULL; // point list to nothing
  array[i] = NULL; // null terminate array
  return array;
}