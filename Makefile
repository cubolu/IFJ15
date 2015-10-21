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
	rm -rf ifj15 test/test *.o $(LOGIN).zip

pack:
	zip $(LOGIN).zip *.c *.h Makefile

test: test/test.c
	cd test; $(CC) -o test test.c -std=c11 $(CHECK) && ./test; exit 0

debug: CFLAGS+= $(DBGFLAGS)
debug: remake

remake: clean ifj15
