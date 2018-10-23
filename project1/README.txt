Use "make" to generate the executable. It is used like so:
./runme [IPC method][textfile][searchstring]
IPC methods are pipe, socket, and sharedmem

main.c will call on the required function located in pipefun.c, socketfun.c, or sharedmem.c


PROJECT CONTRIBUTIONS

Jeremy Duncan
 -Shared memory function
 -Designed the project file system and wrote the Makefile and main.c
 -the report.txt and this README.txt

Nikhila
 -Unix domain socket function

William
 -Pipe function

