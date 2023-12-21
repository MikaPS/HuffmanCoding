# Name of the programs this Makefile is going to build
EXECBIN  = encode decode

# All available .c files are included as SOURCES
SOURCES  = $(wildcard *.c)
# Each .c file has a corresponding .o file
OBJECTS  = $(SOURCES:%.c=%.o)

CC       = clang
CFLAGS   = -Wall -Wpedantic -Werror -Wextra -Ofast -gdwarf-4

.PHONY: all clean spotless format

# built when 'make' is run without arguments.
all: encode decode

# build only encode when calling 'make encode'.
encode: encode.o node.o pq.o code.o io.o stack.o huffman.o
	$(CC) -o $@ $^

# build only decode when calling 'make decode'.
decode: decode.o node.o pq.o code.o io.o stack.o huffman.o
	$(CC) -o $@ $^
	
# This is a default rule for creating a .o file from the corresponding .c file.
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Removes all of the OBJECT files that it can build.
# They can be recreated by running 'make all'.
clean:
	rm -f $(OBJECTS)

# Removes the derived files: the executable itself and
# all of the OBJECT files that it can build.
# They can be recreated by running 'make all'.
spotless:
	rm -f $(EXECBIN) $(OBJECTS)

# Formats all C files based on the clang format. 
format:
	clang-format -i -style=file encode.c
	clang-format -i -style=file decode.c 
	clang-format -i -style=file node.c 
	clang-format -i -style=file pq.c 
	clang-format -i -style=file code.c 
	clang-format -i -style=file io.c 
	clang-format -i -style=file stack.c 
	clang-format -i -style=file huffman.c 
