#include "io.h"
#include "code.h"
#include "defines.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// Goal: handle file actions, such as read and write. Will be used by the
// encoder and decoder to perform actions on files.

// Global variables that will hold the number of bytes read and written
uint64_t bytes_read;
uint64_t bytes_written;

// Static vars
static uint8_t buffer[BLOCK];
static int index_byte = BLOCK;
static uint8_t buffer_code[BLOCK];
static uint32_t index_code;
static int current_bit = 0;

// Read all the specified bytes from a file to the buffer argument buf
int read_bytes(int infile, uint8_t *buf, int nbytes) {
  // Need to read only the number of bytes specified, so stops after reaching
  // this threshold
  int bytes_read_once = 0;
  while (bytes_read_once < nbytes) {
    // Use the library's read function to read one byte at a time
    int r = read(infile, &buf[bytes_read_once], 1);
    // The function returns 0 once it reaches the end of the file
    if (r <= 0) {
      break;
    }
    // Increase the number of bytes read
    bytes_read_once += 1;
  }
  // Add to the total number of bytes_read, the number of bytes read in this
  // function call
  bytes_read += bytes_read_once;
  return bytes_read_once;
}

// Write all the specified bytes from the buffer argument buf to the outfile
// file
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  int bytes_written_once = 0;
  while (bytes_written_once < nbytes) {
    // Use the library's write function to write one byte at a time
    int w = write(outfile, &buf[bytes_written_once], 1);
    // The function returns 0 once it reaches the end of the file
    if (w <= 0) {
      break;
    }
    // Increase the number of bytes written
    bytes_written_once += 1;
  }
  // Add to the total number of bytes_written, the number of bytes written in
  // this function call
  bytes_written += bytes_written_once;
  return bytes_written_once;
}

// Read a block of bytes to a static buffer variable, and return one bit of the
// buffer at a time to the bit argument.
bool read_bit(int infile, uint8_t *bit) {
  //  All bits in the buffer have been doled out
  if (index_byte == BLOCK) {
    // Get a new block of character
    int r = read_bytes(infile, buffer, BLOCK);
    // If no characters were read, we are at the end of the file
    if (r <= 0) {
      return false;
    }
    // reset index since we start from 0
    index_byte = 0;
    current_bit = 0;
  }
  // Get the current bit from the buffer and set it to the bit argument.
  *bit = (buffer[index_byte] & ((1) << (current_bit))) >> current_bit;
  current_bit += 1;
  // Move to the next byte
  if (current_bit > 7) {
    current_bit = 0;
    index_byte += 1;
  }
  return true;
}

// Write the contents of a code to the outfile, only if the number of bits will
// be equal to a size of a BLOCK.
void write_code(int outfile, Code *c) {
  uint32_t current_index = 0;
  // While it is not the end of the code
  while (current_index < code_size(c)) {
    // Get a new bit from the Code. Change it to a character instead of an int.
    uint64_t byte = index_code / 8;
    uint64_t bit = index_code % 8;
    // Set the buffer to the value of the bit
    if (code_get_bit(c, current_index) == true) {
      buffer_code[byte] |= ((1UL) << (bit)); // Sets bit to 1
    } else {
      buffer_code[byte] &= ~((1UL) << (bit)); // Clears bit
    }
    current_index += 1;
    index_code += 1;
    // Write the bytes to a file, and if the bytes equal a full block, reset the
    // index
    if (index_code >= BLOCK * 8) {
      write_bytes(outfile, buffer_code, BLOCK);
      index_code = 0;
    }
  }
}

// Write out any bits that are left over in the buffer after calling the
// write_code function.
void flush_codes(int outfile) {
  // If the current bit is not divisible by 8, then we are in the middle of a
  // byte. In this case, set the current bit to 0 and continue to increase the
  // current bit until it's the end of the byte.
  while (index_code % 8 != 0) {
    buffer_code[index_code / 8] &= ~((1UL) << (index_code));
    index_code += 1;
  }
  // Write the bytes to a file, and since the bytes equal to a full block, reset
  // the index
  write_bytes(outfile, buffer_code, (index_code / 8));
  index_code = 0;
}
