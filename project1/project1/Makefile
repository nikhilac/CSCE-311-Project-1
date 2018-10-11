CC=gcc
CFLAGS=-I.
DEPS = projectheader.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main : main.o pipefun.o socketfun.o sharedmemfun.o
	$(CC) -o runme main.o pipefun.o socketfun.o sharedmemfun.o

clean:
	-rm *.o runme
