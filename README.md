# Intro to the MIPS-Machine
This MIPS machine makes simulation of the MIPS ISA, registers and some memory. It was designed in addition to a more basic machine in order to obtain bonus marks in a university class. The ISA of the machine matches that of MIPS (such as that given in: https://inst.eecs.berkeley.edu/~cs61c/resources/MIPS_Green_Sheet.pdf). This machine will run instructions stored in the zeroth memory address and then the next and next until a "halt" instruction is reached.

In addition to being a MIPS machine, the machine includes translation from deciaml to binary and vice-versa and has the in-built ability to disassemble its memory contents into MIPS assembly language. Disassembly will be done after the initial loading of memory and again for every instruction that is run.

## University Project
The project was a group project that had us code a simulation of a more basic computer. For this more basic machine, I designed and built the fundamental structure of the program (creating structs, fetch-decode-execute cycle and deciding what data types to use), implemented some MIPS instruction, created parsing functions and created functions translating binary and decimal.

About halfway through the project, the group was split so that I would start work on creating a MIPS machine (in order to obtain bonus marks) whilst the others would continue their work on the more basic machine. For that reason, the MIPS machine seen here was developed myself.

For the MIPS machine, I redesigned the fundamentals of the program by representing the new MIPS registers in an array with each index corresponding to its register number. To more easily parse the different types of instructions, I created two new structs: R_Format and I_Format. These structs would be used with new parsing functions I created. These functions would parse each field of a machine instruction, create an instance of the R_Format or I_Format and return that instance. This would allow parsing of a whole instruction in a single line of code.

I reimplemented instructions so that they would now work in the MIPS machine and take advantage of the MIPS registers. Furthermore, read-from-file memory loading, default memory loading and the disassembly functions were revamped to work with the new program.

The default load from memory uses a text file that holds that default content. This default content was designed to be able to recursively calculate Fibonacci numbers (to exemplify the machine's ability to use the stack). This involved coding the program in MIPS machine code. Another text file of machine code was created to demonstrate various instructions and the load-from-file memory loading.

# Using the MIPS-Machine
This MIPS machine takes in three different types of arguments, each of which will load different contents into the memory of the MIPS machine
* "-d" - This loads the default memory into the machine. The code in this case in a Fibonacci calculator which takes advantage of the recursion enabled by this MIPS simulation. The default contents is stored in a txt file of the same directory
* "-c" - Allows the user to input contents (in binary) to sequentially fill the memory, starting from address 0
* "-f" - Takes in memory contents from a txt file specified in the next argument (e.g. ./a.out -f testFunctions.txt)
 
Loading in the memory contents allow loading from a txt file. When making a txt file, the simulation allows for:
* Dashes to be used throughout the binary word whenever desired (except at the very start of a binary word)
* Blank lines or commented lines that begin with a "#"
* Comments in the same line as a binary word, so long as:
  * The binary word comes first
  * There is whitespace between the word and comment
  * For this comment, it is not necessary to start with a "#"
