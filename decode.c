#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// A function used to print the help message
void print_error(void) {
  fprintf(stderr, "SYNOPSIS\n");
  fprintf(stderr, "  A Huffman decoder.\n");
  fprintf(stderr,
          "  Decompresses a file using the Huffman coding algorithm.\n\n");

  fprintf(stderr, "USAGE\n");
  fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n\n");

  fprintf(stderr, "OPTIONS\n");
  fprintf(stderr, "  -h             Program usage and help.\n");
  fprintf(stderr, "  -v             Print compression statistics.\n");
  fprintf(stderr, "  -i infile      Input file to decompress.\n");
  fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
}

int main(int argc, char **argv) {
  int opt = 0; // used for getopt
  // set default numbers
  char input_name[BLOCK] = "stdin";
  char output_name[BLOCK] = "stdout";
  int give_out = 0; // flag to check if a different file was given
  int give_in = 0;
  int stats = 0;

  while ((opt = getopt(argc, argv, "i:o:vh")) != -1) { // list of valid commands
    // sets the name of input file
    if (opt == 'i') {
      give_in = 1;
      strcpy(input_name, optarg);
    }
    // sets the name of the output file
    if (opt == 'o') {
      give_out = 1;
      strcpy(output_name, optarg);
    }
    // enables display of statistics
    if (opt == 'v') {
      stats = 1;
    }
    // usage message
    if (opt == 'h') {
      print_error();
      return 0;
    }
    // if it's not in the above options, return an error number
    if (opt != 'h' && opt != 'v' && opt != 'o' && opt != 'i') {
      print_error();
      return 1;
    }
  }

  // Handle files
  FILE *in = stdin;
  FILE *out = stdout;
  if (give_in == 1) {
    in = fopen(input_name, "r");
  }
  if (give_out == 1) {
    out = fopen(output_name, "w");
  }
  if (!in) { // if there was an error with opening the file
    fprintf(stderr,
            "Couldn't open %s to read plaintext: No such file or directory\n",
            input_name);
    return 1;
  }
  if (!out) { // if there was an error with opening the file
    fprintf(stderr,
            "Couldn't open %s to read plaintext: No such file or directory\n",
            output_name);
    return 1;
  }
  int in_pointer = fileno(in);
  int out_pointer = fileno(out);

  // Header. Gets the header from the input file.
  uint8_t buff[BLOCK];
  read_bytes(in_pointer, buff, sizeof(Header));
  Header *h = (Header *)buff;
  if (h->magic != MAGIC) {
    printf("Invalid magic number.\n");
    return 1;
  }
  // Set the permission of the output file based on the permission written in the input file.
  if (fchmod(out_pointer, h->permissions) != 0) {
    fprintf(stderr, "Chmod error");
  }

  // Gets the dumped tree. Set all the elements to 0.
  uint8_t tree_dump[h->tree_size];
  for (uint64_t i = 0; i < h->tree_size; i += 1) {
    tree_dump[i] = 0;
  }
  read_bytes(in_pointer, tree_dump, h->tree_size);
  Node *root = rebuild_tree(h->tree_size, tree_dump);
  Node *current = root;

  // Go through the deconstructed tree bit by bit. If the bit is 0, then go to the left, and if it's 1 go to the right.
  // When reaching a leaf node, write the node's symbol to outfile. 
  uint8_t bit;
  uint64_t decoded_symbols = 0;
  while (decoded_symbols < h->file_size) {
    if (current) { 
    // Case 1: leaf node
    if (!(current->left) && !(current->right)) {
      decoded_symbols += 1;
      write_bytes(out_pointer, &current->symbol, 1);
      current = root;
    }
    // Read bit
    if (read_bit(in_pointer, &bit)) {
      // Case 2: go to the left
      if (bit == 0) {
        current = current->left;
      }
      // Case 3: go to the right
      if (bit == 1) {
        current = current->right;
      }
    }
    }
  }
  // Statistics, print the compressed file size, the decompress one, and the space saving.
  extern uint64_t bytes_read;
  if (stats == 1) {
    int64_t compressed_size;
    compressed_size = (double)bytes_read;
    double space_saving = 100 * (1 - (compressed_size / (double)h->file_size));
    fprintf(stderr,
            "Compressed file size: %lu bytes\nDecompressed file size: %ld "
            "bytes\nSpace saving: %.2lf%%\n",
            compressed_size, h->file_size, space_saving);
  }
  
  // Delete and close for memory leaks
  delete_tree(&root);
  if (give_in == 1) {
    fclose(in);
  }
  if (give_out == 1) {
    fclose(out);
  }
}
