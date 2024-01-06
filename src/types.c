#include "c.h"

// articles on lvalues and rvalues in C:
// - https://eli.thegreenplace.net/2011/12/15/understanding-lvalues-and-rvalues-in-c-and-c
// - https://jameshfisher.com/2017/01/21/c-lvalue-rvalue/
//
// from my understanding:
// an lvalue is any expression that occupies some identifiable location in memory
// an rvalue is anything that isn't an lvalue
//
// in this code:
// int *p, x;
//
// x is typed int at face value and also as an rvalue:
// x is the address of a cell that holds an int
// x:[int:0]
// int y = x;
//         ^
// using x as an rvalue will essentially mean reading the contents of x
//
// as an lvalue however, it is different since we need to update the contents
// of x's memory. in this case, it'll be a pointer to an int since x is the address
// of the cell
// x:[int:0]
// x = 100;
// x:[int:100], updated the content at the address of x
//
//
// p is typed as pointer to an int at face value and also as an rvalue:
// p is the address of a cell which holds the address of an int cell, y
// p:[int*:y] -------> y:[int:0]
// int *int_pointer = p;
//                    ^
// int_pointer:[int*:y] --|
//                        |
//                        |
//                        ----------> y:[int:0]
//                        |
//                        |
// p:[int*:y] ------------|
// using p as an rvalue will essentially mean reading the contents of p, which
// is another address
//
// as an lvalue however, we will need to update either p (which contains an address)
// or *p which is the memory cell pointed to by the contents of p.
// *p takes the address in p and goes to it (deref). as an lvalue, it will update
// the content of the address being pointed to. as an rvalue, it will read the content
// of the address being pointed to.
//
// p:[int*:y] -------> y:[int:0]
// *p = 420;
// p:[int*:y] -------> y:[int:420]
// this changes the contents of the address p points to, not p's contents
//
// p:[int*:y] -------> y:[int:420], x:[int:100]
// p = &x;
// p:[int*:x] -------> x:[int:100], y:[int:420]
// this changes the contents of p, making it point elsewhere to x by storing x's address

// MACROS

// DATA

// FUNCTIONS
