#include "node.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// The constructor for a node. Creates a new Node and returns a pointer to it if
// the memory was allocated succesfully. Else, return NULL.
// Takes a symbol argument and a frequency argument and set it to the
// corrasponding members of the node.
Node *node_create(uint8_t symbol, uint64_t frequency) {
  // Allocates memory for the new Node
  Node *n = (Node *)malloc(sizeof(Node));
  // If the memory was allocated, set the members of it
  if (n) {
    n->symbol = symbol;
    n->frequency = frequency;
    n->left = NULL;
    n->right = NULL;
  }
  return n;
}

// The destructor for a Node. Frees the pointer to the Node, and set it to NULL.
void node_delete(Node **n) {
  if (*n) { // If the node exists
    free(*n);
    *n = NULL;
  }
}

// Joins a left child ndoe and a right child node, returning a pointer to the
// creater parent node. The frequency of the parent node is equal to the sum of
// the frq of its children (the arguments left and right)
Node *node_join(Node *left, Node *right) {
  if (left && right) { // If left and right exist
    Node *parent = node_create('$', (left->frequency + right->frequency));
    parent->left = left;
    parent->right = right;
    return parent;
  }
  return NULL;
}

// A debug function that is used to print all characteristics of the node.
void node_print(Node *n) {
  if (n) {
    // Checks if it's a printable character, and print the symbol based on that
    if (iscntrl(n->symbol)) {
      printf("Symbol: %02X, Frequency: %lu\n", n->symbol, n->frequency);
    } else {
      printf("Symbol: %c, Frequency: %lu\n", n->symbol, n->frequency);
    }
  }
}

// Compares the frequency of two nodes (the arguments of the function).
// Return true if n is greater than m.
bool node_cmp(Node *n, Node *m) {
  if ((n->frequency) > (m->frequency)) {
    return true;
  } else {
    return false;
  }
}

// Prints the symbol associated with the argument node
void node_print_sym(Node *n) {
  if (iscntrl(n->symbol)) {
    // Checks if it's a printable character, and print the symbol based on that
    printf("Symbol: %02X\n", n->symbol);
  } else {
    printf("Symbol: %c\n", n->symbol);
  }
}
