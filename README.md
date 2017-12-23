# Intro to the MIPS-Machine
This MIPS machine makes full simulation of the MIPS ISA, registers and some memory. It was designed in addition to a more basic machine in order to obtain bonus marks in a university class. This machine will run instructions stored in the zeroth memory address and then the next and next until a "halt" instruction is reached.

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

