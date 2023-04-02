# HuffmanCoding
***How to run the script (encode.c decode.c)***<br>
The first script, encrypt, reads in user input and prints out the compressed file. The second script, decrypt, takes in encrypted inputs and prints out the original file. To compile the script, type in the command line “make”, “make encode”, or “make decode”. Make compiles both scripts, while the other two compile the corresponding programs. Afterward, you can run the program by writing “echo [text] | ./[script]” or “cat [filename] | ./[script]” followed by command line options. For example, to get only the encrypt file, you could write: “cat [filename] | ./encrypt”, and to encrypt the file and then immediately decrypt it, write “cat [filename] | ./encrypt | ./decrypt”.
<br>
***Command Line Options*** <br>
Both scripts have the same command line options. Need to call the script following these options: -i (set the input file). -o (set the output file), -v (enables statistics message), -h (prints help usage message). You can mix and match the command options. For example, you are allowed to call -i -o to set both the input and output files. Inputting other options will lead to an error message.
<br>

***Files***
DESIGN.pdf - shows my general idea and pseudo-code for my code. It has both my initial design and the final one.

README.md - has descriptions on how to run the script, files in the directory, and citations.

Makefile - a script used to compile my sorting file and clean the files after running. You can use it by writing “make {name of function}”.

encode.c - contains the main(). Gets user input from the command line and prints data based on that. Explained in more detail in the command line options section.

decode.c - contains the main(). Gets user input from the command line and prints data based on that. Explained in more detail in the command line options section.

defines.h - a header file that has the definitions of a few variables that will be used in the program.

header.h -  a header file that has the declaration of all the functions and members used in header.c and specifies its interface.

node.h -  a header file that has the declaration of all the functions used in node.c and specifies its interface.

node.c - implements a node that holds the values of a left child, right child, symbol, and frequency.

pq.h -  a header file that has the declaration of all the functions used in pq.c and specifies the interface for priority queue ADT.

pq.c - implements the priority queue, which will hold and sort inserted nodes based on their frequency.

code.h - a header file that has the declaration of all the functions used in code.c and specifies the interface for code ADT.

code.c - implements a code, which will create a unique code for each symbol.

io.h - a header file that has the declaration of all the functions used in io.c and specifies the interface for the io ADT

io.c - implements an io module, which will handle files.
stack.h - a header file that has the declaration of all the functions used in stack.c and specifies the interface for the stack ADT

stack.c - implements a stack, which is used to rebuild the huffman tree.

huffman.h - a header file that has the declaration of all the functions used in huffman.c and specifies the interface for the huffman ADT.

huffman.c - implements functions that are related to the binary trees.
<br>

***Citations***
1)) write() - https://pubs.opengroup.org/onlinepubs/009696699/functions/write.html#:~:text=The%20write()%20function%20shall,return%20errors%20as%20described%20below. 

2)) read() - https://www.tutorialspoint.com/unix_system_calls/read.htm 

3)) Structures - https://www.programiz.com/c-programming/c-structures 

4)) Iscntrl() - https://www.tutorialspoint.com/c_standard_library/c_function_iscntrl.htm 

5)) Ascii to decimal - https://www.includehelp.com/c/convert-ascii-string-to-hexadecimal-string-in-c.aspx 

6)) ASCII Table - https://www.freecodecamp.org/news/ascii-table-hex-to-ascii-value-character-code-chart-2/ 

7)) Bit shift calculator - https://bit-calculator.com/bit-shift-calculator

8)) Size of an array (not used) - https://stackoverflow.com/questions/492384/how-to-find-the-size-of-an-array-from-a-pointer-pointing-to-the-first-element-a 

9)) Write and read functions - https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/ 

10)) fstat() - https://linux.die.net/man/2/fstat
