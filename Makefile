CC=gcc
OBJ=slots.o
CFLAGS=-I. -std=c99 -O3 -lncurses

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	$(CC) -o slots $^ $(CFLAGS)

clean:
	rm -f $(OBJ) slots

.PHONY: clean