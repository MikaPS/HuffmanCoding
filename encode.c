#include "code.h"
#include "defines.h"
#include "header.h"
#include "huffman.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// for fstats
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


/* Functions used for testing:
void node_test(void);
void stack_test(void);
void code_test(void);
void pq_test(void);
void io_test(int infile, int outfile);
void huffman_test(int outfile);*/

// Function to print the help message
void print_error(void) {
  fprintf(stderr, "SYNOPSIS\n");
  fprintf(stderr, "  A Huffman encoder.\n");
  fprintf(stderr,
          "  Compresses a file using the Huffman coding algorithm.\n\n");

  fprintf(stderr, "USAGE\n");
  fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n\n");

  fprintf(stderr, "OPTIONS\n");
  fprintf(stderr, "  -h             Program usage and help.\n");
  fprintf(stderr, "  -v             Print compression statistics.\n");
  fprintf(stderr, "  -i infile      Input file to compress.\n");
  fprintf(stderr, "  -o outfile     Output of compressed data.\n");
}

int main(int argc, char **argv) {
  int opt = 0; // used for getopt
  // set default numbers
  char input_name[BLOCK];
  char output_name[BLOCK];
  int give_out = 0; // flag to check if a different file was given
  int give_in = 0;
  int stats = 0;

  while ((opt = getopt(argc, argv, "i:o:vh")) != -1) { // list of valid commands
    // sets the name of input file
    switch (opt) {
    case 'i':
      give_in = 1;
      strcpy(input_name, optarg);
      break;
    // sets the name of the output file
    case 'o':
      give_out = 1;
      strcpy(output_name, optarg);
      break;
    // enables display of statistics
    case 'v':
      stats = 1;
      break;
    // usage message
    case 'h':
      print_error();
      return 0;

    // if it's not in the above options, return an error number
    default:
      print_error();
      return 1;
    }
  }

  // Handle files
  FILE *in;
  FILE *out = stdout;
  if (give_in == 1) {
    in = fopen(input_name, "r");
  } else {
    // If the input file is stdin, then create a temp file. Copy all bytes from stdin to the temp file
    in = tmpfile();
    if (!in) {
      fprintf(stderr,
              "Couldn't open %s to read plaintext: No such file or directory\n",
              input_name);
      return 1;
    }
    int in_pointer = fileno(in);
    uint8_t temp_buf;
    while (read_bytes(0, &temp_buf, 1) > 0) {
      write_bytes(in_pointer, &temp_buf, 1);
    }
    rewind(in);
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
  int out_pointer = fileno(out);
  int in_pointer = fileno(in);

  // Create a histogram by reading files
  // Set intial values of all characters to 0
  uint64_t hist[ALPHABET];
  for (uint64_t i = 0; i < ALPHABET; i += 1) {
    hist[i] = 0;
  }
  // Read in a byte from infile until reaching the end of the file.
  // Increase the frequency of the corrasponding character in the histogram
  // array
  uint8_t buf = 0;
  while (read_bytes(in_pointer, &buf, 1) > 0) {
    hist[buf] += 1;
  }
  // Ensure that the histogram has at least 2 non-zero values
  if (hist[0] == 0) {
    hist[0] = 1;
  }
  if (hist[1] == 0) {
    hist[1] = 1;
  }

  // Builds a Huffman tree using the histogram, and find its root.
  Node *root = build_tree(hist);
  // Creates a code table
  Code table[ALPHABET];
  for (uint64_t i = 0; i < ALPHABET; i += 1) {
    table[i] = code_init();
  }
  build_codes(root, table);

  // Creates a header
  Header h; // = (Header *)malloc(sizeof(Header));
  // Magic number
  h.magic = MAGIC;
  // Gets information about the file
  struct stat fstats;
  // File permissions and size
  if (fstat(in_pointer, &fstats) == 0) {
    h.permissions = fstats.st_mode;
    if (fchmod(out_pointer, fstats.st_mode) != 0) {
      fprintf(stderr, "chmod error");
    }
    h.file_size = fstats.st_size;
  }
  // Tree size
  h.tree_size = 0;
  for (uint64_t i = 0; i < ALPHABET; i += 1) {
    if (hist[i] > 0) {
      h.tree_size += 1;
    }
  }
  h.tree_size = h.tree_size * 3 - 1;

  // Convert the header to an array of 8bits, and write header to outfile.
  uint8_t *buff = (uint8_t *)&h;
  write_bytes(out_pointer, buff, sizeof(h));
  dump_tree(out_pointer, root);
  // Read from the beginning of infile, and write the symbol for each character
  lseek(fileno(in), 0, SEEK_SET);
  while (read_bytes(in_pointer, &buf, 1) > 0) {
    write_code(out_pointer, &table[buf]);
  }
  flush_codes(out_pointer);
  buf = '\n';
  write_bytes(out_pointer, &buf, 1);

  // Statistics, print the compressed file size, the decompress one, and the space saving.
  if (stats == 1) {
    extern uint64_t bytes_written;
    int64_t compressed_size;
    if (give_in == 1) {
      compressed_size = (double)bytes_written - 1;
    } else {
      // Since we are using write_bytes to copy from stdin to the temp file, need to remove the size of the file from bytes written.
      compressed_size = (double)bytes_written - 1 - (h.file_size);
    }
    double space_saving = 100 * (1 - (compressed_size / (double)h.file_size));
    fprintf(stderr,
            "Uncompressed file size: %lu bytes\nCompressed file size: %ld "
            "bytes\nSpace saving: %.2lf%%\n",
            h.file_size, compressed_size, space_saving);
  }
  
  // Delete and close for memory leaks
  delete_tree(&root);
  fclose(in);
  if (give_out == 1) {
    fclose(out);
  }
  return 0;
}

// TESTS FOR EACH FILE
void huffman_test(int outfile) {
  uint64_t hist[ALPHABET];
  static Code table[ALPHABET];
  for (uint64_t i = 0; i < ALPHABET; i += 1) {
    hist[i] = 0;
  }
  // for (uint64_t i=0; i<20; i+=1) {
  // hist[i] = rand() % 50;
  // printf("i is %lu h[i] is %lu", i, hist[i]);
  //}
  hist['a'] = 1;
  hist['b'] = 4;
  hist['c'] = 2;
  hist['d'] = 2;
  hist['e'] = 4;
  hist['f'] = 1;
  Node *root = build_tree(hist);
  build_codes(root, table);
  printf("tree dump\n");
  dump_tree(outfile, root);
  printf("codes\n");
  for (uint8_t i = 'a'; i < 'g'; i += 1) {
    code_print(&table[i]);
  }
  printf("write code\n");
  for (uint8_t i = 'a'; i < 'g'; i += 1) {
    write_code(outfile, &table[i]);
  }
  flush_codes(outfile);
}

void io_test(int infile, int outfile) {
  printf("-----------IO TEST--------\n");

  // printf("infile is %d\n", infile);
  int size = 40;
  uint8_t buf[size];
  for (int i = 0; i < size; i += 1) {
    buf[i] = 0;
  }
  read_bytes(infile, buf, size);
  /*for (int i=0; i<size; i+=1) {
          printf("byte is %d\n", buf[i]);
  }*/
  write_bytes(outfile, buf, size + 5);
  uint8_t bit = 0;
  while (read_bit(infile, &bit)) {

    printf("bit is %d\n", bit);
  } /*
   Code c = code_init();
   code_push_bit(&c, 1);
   code_push_bit(&c, 1);
   code_push_bit(&c, 0);
   write_code(outfile, &c);*/
}

void pq_test(void) {
  printf("-------TEST PRIORITY QUEUE--------\n");
  PriorityQueue *q = pq_create(5);
  printf("is queue empty? %d\nis queue full? %d\n", pq_empty(q), pq_full(q));
  printf("queue size: %u\n", pq_size(q));
  pq_print(q);
  Node *n = node_create('c', 5);
  Node *m = node_create('%', 31);
  Node *x = node_create('d', 7);
  Node *y = node_create('p', 5);
  Node *z = node_create('p', 423);
  Node *u = node_create('u', 3);
  Node *w = node_create('w', 18);
  Node *a = node_create('a', 9);
  Node *b = node_create('b', 1);
  printf("adding n...\n");
  enqueue(q, n);
  // pq_print(q);
  printf("adding m...\n");
  enqueue(q, m);
  // pq_print(q);
  printf("adding x and y...\n");
  enqueue(q, x);
  enqueue(q, y);
  // pq_print(q);
  printf("adding z and u...\n");
  enqueue(q, z);
  enqueue(q, u);
  // pq_print(q);
  enqueue(q, w);
  enqueue(q, a);
  enqueue(q, b);
  pq_print(q);

  printf("DEQUEUE--\n");
  Node *bit = NULL;
  dequeue(q, &bit);
  printf("bit is \n");
  node_print(bit);
  printf("current queue\n");
  pq_print(q);
}

void code_test(void) {
  printf("-----TEST CODE-----\n");
  Code c = code_init();
  printf("is code empty? %d\nis code full? %d\n", code_empty(&c),
         code_full(&c));
  printf("code size: %u\n", code_size(&c));
  printf("adding a bit..\n");
  code_push_bit(&c, 0);
  code_push_bit(&c, 0);
  code_push_bit(&c, 1);
  code_print(&c);
  code_set_bit(&c, 1);
  code_print(&c);
  code_push_bit(&c, 1);
  code_push_bit(&c, 0);
  code_print(&c);
  code_clr_bit(&c, 2);
  code_print(&c);
  //	code_clr_bit(&c, 1);
  //	code_print(&c);
  //	code_push_bit(&c, 0);
  // code_set_bit(&c, 1);
  // code_print(&c);
  // code_push_bit(&c, 1);
  // code_print(&c);
  /*	uint8_t b = 0;
          code_pop_bit(&c, &b);
          code_print(&c);
          printf("the popped bit is %d\n", b);
          code_push_bit(&c, 0);
          code_print(&c);*/
}

void stack_test(void) {
  printf("-----TEST STACK-----\n");
  Stack *s = stack_create(10);
  printf("is stack empty? %d\nis stack full? %d\n", stack_empty(s),
         stack_full(s));
  printf("stack size: %u\n", stack_size(s));
  Node *n = node_create('c', 5);
  Node *m = node_create('%', 31);
  stack_push(s, n);
  stack_push(s, m);
  printf("added two nodes..\n");
  stack_print(s);
  Node *p;
  printf("removed a node..\n");
  stack_pop(s, &p);
  stack_print(s);
  printf("pop node:\n");
  node_print(p);
  Node *x = node_create('d', 12);
  stack_push(s, x);
  stack_print(s);
  stack_delete(&s);
  node_delete(&p);
}

void node_test(void) {
  printf("-----TEST NODE-----\n");
  Node *n = node_create('c', 5);
  Node *m = node_create('%', 31);
  printf("n: ");
  node_print(n);
  printf("m: ");
  node_print(m);
  Node *parent = node_join(n, m);
  printf("parent: ");
  node_print(parent);
  printf("compare n and m: %d\n", node_cmp(n, m));
}
