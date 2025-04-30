CC = gcc
CFLAGS = -Wall -std=gnu11 -g -lm
BIN = ttytris

OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) *.o -o $(BIN)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(BIN)
	rm -f *.o

