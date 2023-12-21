#include "pq.h"
#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Goal: a queue where each element has a priority, so elements with higher
// priority will dequeue earlier. In my implementation, I used insertion sort
// and put the nodes with highest freqeuncies at the end of an array. That way,
// when dequeueing a node, all we need to do is to decrease the size of the
// array without changing the positions of the other nodes.

// Defines what members/fields the Stack structure has.
// Size represents the number of elements currently in the queue.
// Capacity is the maximum number of nodes that can be includes in the queue.
// Items is an array of nodes.
typedef struct PriorityQueue PriorityQueue;

struct PriorityQueue {
  uint32_t size;
  uint32_t capacity;
  Node **items;
};

// Constructor for a PriorityQueue. Creates a new PriorityQueue and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL.
// Takes an uint32_t argument capacity and set the member capacity of the
// structure to it.
PriorityQueue *pq_create(uint32_t capacity) {
  // Allocates memory for the new PriorityQueue
  PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  // If the memory was allocated, set the members of the PriorityQueue
  if (q != NULL) {
    q->capacity = capacity;
    q->size = 0;
    q->items = (Node **)calloc(q->capacity, sizeof(Node *));
    if (!q->items) {
      free(q);
      q = NULL;
    }
  }
  // Returns the PriorityQueue
  return q;
}

// The destructor for a PriorityQueue. Frees the pointer to the PriorityQueue,
// and set it to NULL.
void pq_delete(PriorityQueue **q) {
  if (*q) {
    // Deletes all nodes in the item array
    for (uint32_t i = 0; i < (*q)->size; i += 1) {
      node_delete(&(*q)->items[i]);
    }
    // Deletes the array itself
    free((*q)->items);
    (*q)->items = NULL;
    // deletes the queue
    free(*q);
    *q = NULL;
  }
}

// Checks if the PriorityQueue is empty. Returns true if empty, false otherwise
bool pq_empty(PriorityQueue *q) {
  // The size member represents how many elements were added into the
  // PriorityQueue. It increases whenever new elements are added, so if it's
  // zero, then the PriorityQueue is empty.
  if (q->size == 0) {
    return true;
  }
  return false;
}

// Checks if the PriorityQueue is full. Returns true if full, false otherwise.
bool pq_full(PriorityQueue *q) {
  // The capacity is the maximum number of elements that can be added to the
  // PriorityQueue. So, if the size is equal to the capcity-1, the PriorityQueue
  // is full. We need to do -1 since size starts with the index 0.
  if (q->size == (q->capacity)) {
    return true;
  }
  return false;
}

// Returns the number of elements in the queue.
uint32_t pq_size(PriorityQueue *q) { return (q->size); }

// Inserts a node into the PriorityQueue. Returns true to indicate success,
// false otherwise.
bool enqueue(PriorityQueue *q, Node *n) {
  // Ensures that the queue exists and it's not full
  if (q && n) {
    if (!pq_full(q)) {
      // Special case: the queue is empty.
      // Adds the node at location 0 and increase the size of the queue
      if (q->size == 0) {
        q->items[0] = n;
        q->size += 1;
        return true;
      }
      // If the queue has at least one element
      else {
        // Set the new element at the end of the queue
        q->items[q->size] = n;
        // Goes through all elements of the queue
        for (uint32_t i = 0; i < q->size; i += 1) {
          // If the current element is smaller than the next element
          // Swap them
          if (node_cmp(q->items[i], q->items[i + 1]) == 0) {
            // swap
            Node *temp = q->items[i + 1];
            q->items[i + 1] = q->items[i];
            q->items[i] = temp;
            // Sorts the left side of the array after the swap
            for (uint32_t j = i; j > 0; j -= 1) {
              // If the current element is bigger than the previous one, swap
              // them
              if (node_cmp(q->items[j], q->items[j - 1]) == 1) {
                Node *temp = q->items[j - 1];
                q->items[j - 1] = q->items[j];
                q->items[j] = temp;
              } else {
                break;
              }
            }
          }
        }
        q->size += 1;
        return true;
      }
    }
  }
  return false;
}

// Dequeues a node from the PriorityQueue. Returns true to indicate success,
// false otherwise.
bool dequeue(PriorityQueue *q, Node **n) {
  // Takes the item at the end of the PriorityQueue and inserts it into the
  // argument n Decreases the size of the queue by 1
  if (q) {
    if (!pq_empty(q)) {
      *n = q->items[q->size - 1];
      q->size -= 1;
      return true;
    }
    return false;
  }
  return false;
}

// A debug function that prints all characteristics of the PriorityQueue.
void pq_print(PriorityQueue *q) {
  printf("Size: %u, Capacity: %u\n", q->size, q->capacity);
  // Go over all nodes in the item list and prints them.
  for (uint32_t i = 0; i < q->size; i += 1) {
    node_print(q->items[i]);
  }
}
