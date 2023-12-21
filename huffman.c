#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Goal: an interface for the provided Huffman coding module.

// Creates a static Code variable that will hold a vale through recursion calls
// on the build_codes function
static Code c = {0, {0}};

// Creates a tree given given a histogram (using a priority queue).
Node *build_tree(uint64_t hist[static ALPHABET]) {
  // Step 1: creates a priority queue and populate it using the histogram
  PriorityQueue *pq = pq_create(ALPHABET);
  // Go through each element in the histogram, and create a node that is
  // associated with it
  for (uint64_t i = 0; i < ALPHABET; i += 1) {
    if (hist[i] > 0) {
      Node *n = node_create(i, hist[i]);
      enqueue(pq, n);
    }
  }
  // Step 2: construct a huffman tree
  // As long as there are more than two elements at the priority queue
  while (pq_size(pq) > 1) {
    // Get the left and right children and join them
    Node *left;
    dequeue(pq, &left);
    Node *right;
    dequeue(pq, &right);
    Node *parent = node_join(left, right);
    // Add the joined node to the priority queue.
    // Add the left and right children & the joined node to the tree
    enqueue(pq, parent);
  }
  // Return the root of the tree (last element in the priority queue)
  Node *root;
  dequeue(pq, &root);
  pq_delete(&pq);
  return root;
}

// Creates a code table based on the Huffman tree we built.
// Takes as arguments the root of the tree and an empty code table to be filled
// out.
void build_codes(Node *root, Code table[static ALPHABET]) {
  // If the node exists
  if (root != NULL) {
    // If we are at a leaf, set the current location of the table to the code
    if ((!root->left) && (!root->right)) {
      table[root->symbol] = c;
    } else {
      // If we are going to the left, push the number 0
      code_push_bit(&c, 0);
      build_codes(root->left, table);
      // Remove the node we already found a code for
      uint8_t b = 0;
      code_pop_bit(&c, &b);
      // If we are going t the right, push the number 1
      code_push_bit(&c, 1);
      build_codes(root->right, table);
      code_pop_bit(&c, &b);
    }
  }
}

// Creates a string representation of the tree and write it to outfile.
void dump_tree(int outfile, Node *root) {
  // If the node exists
  if (root) {
    // Use post order traversal to go through the tree
    dump_tree(outfile, root->left);
    dump_tree(outfile, root->right);
    // If we are at the leaf, push the character L and the node's symbol
    if (!root->left && !root->right) {
      uint8_t buf = 'L';
      write_bytes(outfile, &buf, 1);
      write_bytes(outfile, &root->symbol, 1);
    }
    // Else, we are in an interior node, push the character I
    else {
      uint8_t buf = 'I';
      write_bytes(outfile, &buf, 1);
    }
  }
}

// Recobstructs the Huffman tree based on the given tree dump
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
  // Create a stack
  Stack *s = stack_create(nbytes);
  for (uint32_t i = 0; i < nbytes; i += 1) {
    // Leaf node: push the next element of the tree to the stack
    if (tree[i] == 'L') {
      Node *n = node_create(tree[i + 1], 0); 
      stack_push(s, n);
      i += 1;
    }
    // Interior node: joins the top two nodes of the stack
    else if (tree[i] == 'I') {
      Node *right;
      stack_pop(s, &right);
      Node *left;
      stack_pop(s, &left);
      Node *parent = node_join(left, right);
      stack_push(s, parent);
    }
  }
  // Returns the root of the tree, it is the last node in the stack
  Node *root;
  stack_pop(s, &root);
  stack_delete(&s);
  return root;
}

// The destructor for the tree. Use post-order traversal to delete all the nodes in the tree.
// Start from left child, move to right child, and then delete the node.
void delete_tree(Node **root) {
  if ((*root)->left) {
    delete_tree(&(*root)->left);
  }
  if ((*root)->right) {
    delete_tree(&(*root)->right);
  }
  node_delete(root);
}
