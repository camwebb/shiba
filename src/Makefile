CC = gcc 
HDRS	= shiba.h
OBJS = main.o newick.o io.o
LIBS	= -lm -lmxml -lpthread
CFLAGS = -Wall -g -std=gnu11 -D_THREAD_SAFE -D_REENTRANT -O0 # -O3

all: shiba

shiba: $(OBJS)
	$(CC) $(CFLAGS) -o shiba $(OBJS) $(LIBS)

main.o: main.c $(HDRS)

newick.o: newick.c $(HDRS)

io.o: io.c $(HDRS)

clean:
	rm -f *.o

doc: Doxyfile
	doxygen Doxyfile

