# Makefile for shiba

CC = gcc -Wall -g -O1
HDRS	= land.h lineage.h shiba.h # nrutil.h
OBJS = shiba.o
LIBS	= -lm


all: shiba

shiba: $(OBJS)
	$(CC) -o shiba $(OBJS) $(LIBS)

shiba.o: shiba.c $(HDRS)

clean:
	rm -f *.o

distrib:
	mkdir shiba-dist
	cp shiba.c shiba.h land.h lineage.h Makefile shiba-dist
	tar cvzf shiba.tgz shiba-dist
	mv shiba.tgz ..
	rm -rf shiba-dist

backup:
	git commit -a -m "make backup = auto"
	git push cwebb@phylodiversity.net:git/shiba/ master
