#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Goal:
// The stack will be made as a list, where each node contains a value and can
// point to its left and right children.

// Defines what members/fields the Stack structure has.
// Top represents the top of the stack.
// Capacity is the maximum number of nodes that can be includes in the stack.
// Items is an array of nodes.
typedef struct Stack Stack;

struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
};

// The constructor for the Stack. Creates a new Stack and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL.
// Takes an uint32_t argument capacity and set the member capacity of the
// structure to it.
Stack *stack_create(uint32_t capacity) {
  // Allocates memory for the new Stack
  Stack *s = (Stack *)malloc(sizeof(Stack));
  // If the memory was allocated, set the members of the Stack
  if (s != NULL) {
    s->capacity = capacity;
    s->top = 0;
    s->items = (Node **)calloc(s->capacity, sizeof(Node *));
    if (!s->items) {
      free(s);
      s = NULL;
    }
  }
  // Returns the Stack
  return s;
}

// The destructor for a Stack. Frees the pointer to the Stack, and set it to
// NULL
void stack_delete(Stack **s) {
  if (*s) {
    // Deletes all nodes in the item array
    for (uint32_t i = 0; i < (*s)->top; i += 1) {
      node_delete(&(*s)->items[i]);
    }
    // Deletes the array itself
    free((*s)->items);
    (*s)->items = NULL;
    // Deletes the stack
    free(*s);
    *s = NULL;
  }
}

// Checks if the stack is empty. Returns true if empty, false otherwise
bool stack_empty(Stack *s) {
  // The top member represents how many elements were added into the stack.
  // It increases whenever new elements are added, so if it's zero, then the
  // stack is empty.
  if (s->top == 0) {
    return true;
  }
  return false;
}

// Checks if the stack is full. Returns true if full, false otherwise.
bool stack_full(Stack *s) {
  // The capacity is the maximum number of elements that can be added to the
  // stack. So, if the top is equal to the capcity-1, the stack is full. We need
  // to do -1 since top starts with the index 0.
  if (s->top == (s->capacity) - 1) {
    return true;
  }
  return false;
}

// Returns the number of nodes in the stack.
uint32_t stack_size(Stack *s) { return (s->top); }

// Pushes a new node into the stack. Returns true to indicate success, false
// otherwise
bool stack_push(Stack *s, Node *n) {
  // If the stack is full, can't add any more items and return false.
  if (!stack_full(s)) {
    // If it's not full, set the top of the stack to the new node
    // Increase the value of the top by 1
    s->items[s->top] = n;
    s->top += 1;
    return true;
  }
  return false;
}

// Pops a node off the stack (remove the top node). Returns true to indicate
// success, false otehrwise.
bool stack_pop(Stack *s, Node **n) {
  // If the stack is empty, can't remove any more items and return false.
  if (!stack_empty(s)) {
    // If it's not empty, set the pointer to the node argument to the top item.
    // Decrease the value of top by 1.
    *n = s->items[s->top - 1];
    s->top -= 1;
    return true;
  }
  return false;
}

// A debug function that prints all characteristics of the stack.
void stack_print(Stack *s) {
  printf("Top: %u, Capacity: %u\n", s->top, s->capacity);
  // Go over all nodes in the item list and prints them.
  for (uint32_t i = 0; i < s->top; i += 1) {
    node_print(s->items[i]);
  }
}
