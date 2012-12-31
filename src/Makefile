CC = gcc 
HDRS	= shiba.h nrutil.h
OBJS = nrutil.o shiba.o io.o
LIBS	= -lm -lmxml -lpthread
# xml2-config --cflags
CFLAGS = -Wall -g -O3 -std=gnu11 -D_THREAD_SAFE -D_REENTRANT

all: shiba

shiba: $(OBJS)
	$(CC) $(CFLAGS) -o shiba $(OBJS) $(LIBS)

shiba.o: shiba.c $(HDRS)

io.o: io.c $(HDRS)

nrutil.o: nrutil.c $(HDRS)

clean:
	rm -f *.o

docs: Doxyfile
	doxygen Doxyfile
