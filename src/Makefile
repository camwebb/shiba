CC = gcc 
HDRS	= shiba.h
OBJS = shiba.o newick.o io.o
LIBS	= -lm -lmxml -lpthread
CFLAGS = -Wall -g -O3 -std=gnu11 -D_THREAD_SAFE -D_REENTRANT

all: shiba

shiba: $(OBJS)
	$(CC) $(CFLAGS) -o shiba $(OBJS) $(LIBS)

shiba.o: shiba.c $(HDRS)

newick.o: newick.c $(HDRS)

io.o: io.c $(HDRS)

clean:
	rm -f *.o

docs: Doxyfile
	doxygen Doxyfile

