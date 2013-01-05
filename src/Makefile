CC = gcc
HDRS	= shiba.h version.h
OBJS = main.o newick.o io.o shiba.o
LIBS	= -lm -lmxml -lpthread
CFLAGS = -Wall -g -std=gnu11 -D_THREAD_SAFE -D_REENTRANT -O3 # -O0 for gdb

all: shiba

shiba: $(OBJS)
	$(CC) $(CFLAGS) -o shiba $(OBJS) $(LIBS)

main.o: main.c $(HDRS)

shiba.o: shiba.c $(HDRS)

newick.o: newick.c $(HDRS)

io.o: io.c $(HDRS)

clean:
	rm -f *.o

website: doc
	rsync -avP --delete ../doc/html/ cwebb@phylodiversity.net:www/shiba/ 

doc: Doxyfile version.txt
	doxygen Doxyfile
	rm -f version.txt

version.h: .PHONY
	echo '#define VERSION "'`git tag`'"' > version.h

.PHONY:

version.txt:
	echo '<b>Version '`git tag`'</b>' > version.txt
