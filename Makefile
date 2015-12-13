CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -DNDEBUG
DBGFLAGS = -g
DBGINFO = -UNDEBUG
GPFLAGS = -pg
LOGIN = xluzny00
CHECK = `pkg-config --cflags --libs check`

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

ifj15: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean pack debug test

clean:
	rm -f ifj15 test/test *.o *.gch gmon.out $(LOGIN).zip
	@cd test; make clean

pack:
	zip $(LOGIN).zip *.c *.h Makefile rozdeleni dokumentace.pdf

test: ifj15
	@cd test; make run

debug: CFLAGS+= $(DBGFLAGS) $(DBGINFO)
debug: remake

gprof: CFLAGS+= $(GPFLAGS) $(DBGFLAGS)
gprof: remake

remake: clean ifj15
