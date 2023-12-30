#include "c.h"

// MACROS
#define HASHSIZE NELEMS(((Table)0)->buckets)

// DATA
static struct table cns = { CONSTANTS },
ext = { GLOBAL }, ids = { GLOBAL },
tys = { GLOBAL };

Table constants = &cns;
Table externals = &ext;
Table identifiers = &ids; // points to identifiers at current scope
Table globals = &ids; // points to identifiers at global scope
Table types = &tys;
Table labels;

// FUNCTIONS
Table new_table(Table parent, int lvl) {
  Table child;
  NEW0(child, FUNC);
  child->previous = parent;
  child->level = lvl;
  if (parent) {
    child->all = parent->all;
  }
  return child;
}