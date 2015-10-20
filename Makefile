CC = gcc
CFLAGS = -std=c11 -Wall
DBGFLAGS = -g
LOGIN = xabcde00

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS)

ifj15: $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

.PHONY: clean pack debug

clean:
	rm -f ifj15 *.o $(LOGIN).zip

pack:
	zip $(LOGIN).zip *.c *.h Makefile

debug: CFLAGS+= $(DBGFLAGS)
debug: remake

remake: clean ifj15
