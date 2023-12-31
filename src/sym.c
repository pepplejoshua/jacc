#include "c.h"

// MACROS
#define HASHSIZE NELEMS(((Table)0)->buckets)
#define equalp(x) v.x == p->sym.additional_info.constant_info.val.x

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
int level = GLOBAL;

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

void foreach(Table tp, int lvl, void(*apply)(Symbol, void *), void *closure) {
  while (tp && tp->level > lvl) tp = tp->previous; // go to correct scope level
  if (tp && tp->level == lvl) { // if we do find the right scope level
    Symbol p;
    Coordinate save = src; // track the current src location
    // move through all the symbols at this scope level
    for (p = tp->all; p && p->scope == lvl; p = p->up) {
      src = p->src; // track the location of the current symbol for error reporting
      (*apply)(p, closure); // apply the passed function
    }
    src = save; // return back to tracked coordinate
  }
}

void enterScope() {
  ++level;
}

void exitScope() {
  // remove all type cache types with tags defined in the vanishing scope
  // this is implemented and clarified in section 4.2
  rmtypes(level);
  // exit the types scope if it is the current level
  if (types->level == level)
    types = types->previous;
  if (identifiers->level == level) {
    // warn if there are more than 127 identifiers
    identifiers = identifiers->previous;
  }
  --level;
}

Symbol install(char *name, Table *tpp, int level, int arena) {
  Table tp = *tpp; // deref tpp to get the actual table pointer
  struct entry *p;
  // compute hash of the symbol name to a restricted range
  unsigned h = (unsigned)name & (HASHSIZE - 1);

  if (level > 0 && tp->level < level)
    tp = *tpp = new_table(tp, level);

  // allocate and set member fields
  NEW0(p, arena);
  p->sym.name = name;
  p->sym.scope = level;
  // update tp->all chain to include new symbol
  p->sym.up = tp->all;
  tp->all = &p->sym;
  // point the start of the hash chain to the new entry
  p->link = tp->buckets[h];
  tp->buckets[h] = p;
  return &p->sym;
}

Symbol lookup(char *name, Table tp) {
  struct entry *p;
  // compute hash of the symbol name to a restricted range
  unsigned h = (unsigned)name & (HASHSIZE - 1);

  // walking up the scopes from the current scope, we will
  // pick the right hash chain in each scope and try to
  // find the symbol
  do {
    for (p = tp->buckets[h]; p; p = p->link) { // walk the symbols in the right index, from most recent
      if (name == p->sym.name) {
        return &p->sym;
      }
    }
  } while ((tp = tp->previous) != NULL); // walk the scopes from in to out
  return NULL;
}

// used to manage compiler generated labels as well as internal
// source language labels
int genLabel(int n) {
  static int label = 1;

  label += n;
  return label - n;
}

// this creates and manages labels table for internal source language labels
// it tries to find the matching sym for a label if any, else it makes a new one
Symbol findLabel(int lab) {
  struct entry *p;
  unsigned h = lab & (HASHSIZE - 1);

  for (p = labels->buckets[h]; p; p = p->link) {
    if (lab == p->sym.additional_info.label_info.label) {
      return &p->sym;
    }
  }
  NEW0(p, FUNC); // create new entry for label symbol
  p->sym.name = stringd(lab);
  p->sym.scope = LABELS;
  // update labels table `all` chain with new sym
  p->sym.up = labels->all;
  labels->all = &p->sym;
  // update the link with the hash chain bucket
  p->link = labels->buckets[h];
  labels->buckets[h] = p;
  // update label specific info
  p->sym.generated = 1;
}

Symbol constant(Type ty, Value v) {
  struct entry *p;
  unsigned h = v.u & (HASHSIZE - 1);

  // unequal returns the unqualified version of a type
  // i.e. without volatile or const.
  ty = unequal(ty);
  for (p = constants->buckets[h]; p; p = p->link) {
    if (eqtype(ty, p->sym.type, 1)) {
      // return the symbol if p's value == v's
      switch (ty->op) {
      case CHAR: {
        if (equalp(uc)) return &p->sym;
        break;
      }
      case SHORT: {
        if (equalp(ss)) return &p->sym;
        break;
      }
      case INT: {
        if (equalp(i)) return &p->sym;
        break;
      }
      case UNSIGNED: {
        if (equalp(u)) return &p->sym;
        break;
      }
      case FLOAT: {
        if (equalp(f)) return &p->sym;
        break;
      }
      case DOUBLE: {
        if (equalp(d)) return &p->sym;
        break;
      }
      case ARRAY:
      case FUNCTION: {
        break;
      }
      case POINTER: {
        break;
      }
      }
    }
  }
  NEW0(p, PERM);
  p->sym.name = vtoa(ty, v);
  p->sym.scope = CONSTANTS;
  p->sym.type = ty;
  p->sym.sclass = STATIC;
  p->sym.additional_info.constant_info.val = v;
  p->link = constants->buckets[h];
  p->sym.up = constants->all;
  constants->all = &p->sym;
  constants->buckets[h] = p;
  // announce the constant, if necessary (page 49)
  p->sym.defined = 1;
  return &p->sym;
}

char *vtoa(Type ty, Value v) {}