CC=gcc
CFLAGS=-std=c99 -Werror -Wall -Wextra -Wvla

OBJ=epoll.o
BIN=epoll

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean