# Makefile for the simplified ls(1) (works with BSD make and GNU make)
PROG = myls
OBJS = main.o options.o entry.o sort.o print.o util.o format.o longfmt.o
CC = cc
CFLAGS = -Wall -Wextra -O2

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $<

main.o: main.c ls.h options.h entry.h sort.h print.h util.h
options.o: options.c options.h ls.h
entry.o: entry.c entry.h ls.h util.h
sort.o: sort.c sort.h entry.h ls.h format.h
print.o: print.c print.h entry.h ls.h format.h longfmt.h util.h
util.o: util.c util.h
format.o: format.c format.h entry.h ls.h
longfmt.o: longfmt.c longfmt.h

clean:
	rm -f $(PROG) $(OBJS)

.PHONY: all clean
