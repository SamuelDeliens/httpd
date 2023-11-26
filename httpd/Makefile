export CC = gcc
export CPPFLAGS = -Isrc
export CFLAGS = -std=c99 -Werror -Wall -Wextra -Wvla -pedantic -fsanitize=address -g

BINARY=httpd

all: $(BINARY)

$(BINARY):
	$(MAKE) -C src
	mv src/$(BINARY) .

clean:
	rm -f $(BINARY)
	$(MAKE) -C src clean

.PHONY: all
