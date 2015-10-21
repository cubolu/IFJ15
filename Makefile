CC = gcc
CFLAGS = -std=c11 -Wall
DBGFLAGS = -g
LOGIN = xabcde00
CHECK = `pkg-config --cflags --libs check`

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

ifj15: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean pack debug test

clean:
	rm -f ifj15 test/test *.o $(LOGIN).zip
	@cd test; make clean

pack:
	zip $(LOGIN).zip *.c *.h Makefile

test: ifj15
	@cd test; make run

debug: CFLAGS+= $(DBGFLAGS)
debug: remake

remake: clean ifj15
