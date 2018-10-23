# CSCE-311-Project-1
Pipe: Written by William Detyens
Created using 3 unidirectional pipes to pass information to and from the parent and child
Parent opens and reads from a given file into a character array. The parent then writes to the 
child the char array and the array size via pipe. The child process then parses the string using
strtok() and selects the individual sentences containing the key word from the command line. 
Once the child is ready to return the selected sentences, another pipe is used to read the string 
To the parent. 
Command line entry: ./runme pipe [filename] [searchword]
