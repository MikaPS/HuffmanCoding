#include "code.h"
#include "defines.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Goal:
// A stack of bits that create a unique code for each ASCII symbol used

// The constructor for the Code. Creates a new Code, set its top member to 0,
// and zeroes out its bits array.
Code code_init(void) {
  Code c;
  c.top = 0;
  // In the header file, used MAX_CODE_SIZE to intialize bits
  for (int i = 0; i < MAX_CODE_SIZE; i += 1) {
    c.bits[i] = 0;
  }
  return c;
}

// Returns the size of Code
uint32_t code_size(Code *c) {
  // Since the top of a stack keeps track of the number of elements in the
  // stack, the top member represents the number of pushed bits.
  return (c->top);
}

// Checks if the Code is empty. Returns true if empty, false otherwise
bool code_empty(Code *c) {
  // The top member represents how many elements were added into the Code.
  // It increases whenever new elements are added, so if it's zero, then the
  // Code is empty.
  if (c->top == 0) {
    return true;
  }
  return false;
}

// Checks if the Code is full. Returns true if full, false otherwise.
bool code_full(Code *c) {
  // The number of possible ASCII characters is defined in the macro ALPHABET
  // which is the maximum capacity for items in Code. So, if the top is equal to
  // the ALPHABET-1, the Code is full. We need to do -1 since top starts with
  // the index 0.
  if (c->top == ALPHABET - 1) {
    return true;
  }
  return false;
}

// Sets the bit at index i. Returns true to indicate success, false otherwise.
bool code_set_bit(Code *c, uint32_t i) {
  if (c != NULL) {
    uint64_t byte = i / 8;
    // Checks that the byte is in range
    if (byte > MAX_CODE_SIZE) {
      return false;
    }
    uint64_t bit = i % 8;
    // Checks that the bit is in range
    if (bit > ALPHABET) {
      return false;
    }
    // Since we want to set the value, the mask is 1
    // Use OR (|) because if the current value is 0, it will switch to 1 (0|1=1,
    // 1|1=1) Use shifting to get the right bit inside of the byte
    // printf("bit is %lu, byte is %lu, and current bit at byte is %d\n", bit,
    // byte, c->bits[byte]); printf("moving 1 bit times: %lu\n", (1UL <<
    // (i%8)));
    c->bits[byte] |= ((1UL) << (bit));
    return true;
  }
  return false;
}

// Clears the bit at index i. Returns true to indicate success, false otherwise.
bool code_clr_bit(Code *c, uint32_t i) {
  if (c != NULL) {
    uint32_t byte = i / 8;
    // Checks that the byte is in range
    if (byte > MAX_CODE_SIZE) {
      return false;
    }
    uint32_t bit = i % 8;
    // Checks that the bit is in range
    if (bit > ALPHABET) {
      return false;
    }
    // Use AND (&) because 0&1=0 and 0&0=0, so the chosen bit will become 0
    // Shift the bit 1, bit amount of places
    // Use shifting to get the right bit inside of the byte
    c->bits[byte] &= ~((1UL) << (bit));
    return true;
  }
  return false;
}

// Gets the bit at index i. Returns true if the bit is set to 1, false
// otherwise.
bool code_get_bit(Code *c, uint32_t i) {
  if (c != NULL) {
    uint32_t byte = i / 8;
    // Checks that the byte is in range
    if (byte > MAX_CODE_SIZE) {
      return false;
    }
    uint32_t bit = i % 8;
    // Checks that the bit is in range
    if (bit > ALPHABET) {
      return false;
    }
    // Use shifting to get the right bit
    // Use &1UL because if the value is 1, it will just return 1. Else, it will
    // return 0
    if (((c->bits[byte] & ((1UL) << (bit))) >> bit) == 1) {
      return true;
    }
  }
  return false;
}

// Pushes a new bit onto Code, the bit will be added to the top. Returns true to
// indicate success, false otherwise.
bool code_push_bit(Code *c, uint8_t bit) {
  // uint32_t byte_change = (c->top)/8;
  // uint32_t bit_change = (c->top)%8;
  //  If the Code is full, can't add any more items and return false.
  if (!code_full(c)) {
    // If it's not full, set the top of the code to the new node.
    // Use the same shifting mechanics as set bit, but instead of 1UL use the
    // bit argument value.
    // Increase the value of the top by 1.
    // c->bits[c->top] = bit;
    if (bit == 1) {
      code_set_bit(c, c->top);
    } else {
      code_clr_bit(c, c->top);
    }
    c->top += 1;
    return true;
  }
  return false;
}

// Pops a node off the Code (remove the top bit). Returns true to indicate
// success, false otehrwise.
bool code_pop_bit(Code *c, uint8_t *bit) {
  // If the Code is empty, can't remove any more items and return false.
  if (!code_empty(c)) {
    // If it's not empty, set the pointer to the bit argument to the top item.
    // Decrease the value of top by 1.
    *bit = code_get_bit(c, c->top - 1); // c->bits[c->top-1];
    c->top -= 1;
    return true;
  }
  return false;
}

// A debug function that prints all the characteristics of Code
void code_print(Code *c) {
  for (uint32_t i = 0; i < c->top; i += 1) {
    // Boolean values are converted to binary (1 and 0), so can use an int
    // specifier.
    printf("%d", code_get_bit(c, i));
    // printf("%d", c->bits[i]);
  }
  printf("\n");
}
