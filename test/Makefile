CC = gcc
CFLAGS = -std=c11 -g -Wno-int-conversion -Wno-pointer-to-int-cast
CHECKLIB = `pkg-config --cflags --libs check`
INCLUDE = -I..
LIBDIR = -L..

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
PARENTSRC = $(filter-out ../ifj15.c, $(wildcard ../*.c))
PARENTOBJ = $(patsubst %.c,%.o,$(PARENTSRC))

%.o : %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

test: $(OBJ) $(PARENTOBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(LIBDIR) $(CHECKLIB)

.PHONY: clean run

clean:
	rm -f test *.o

pack:
	zip $(LOGIN).zip *.c *.h Makefile

run: test
	@./test; exit 0

remake: clean test
