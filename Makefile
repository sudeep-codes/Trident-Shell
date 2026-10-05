CC      := gcc
CFLAGS  := -std=c11 -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Werror -g
SRC     := $(wildcard src/*.c)
OBJ     := $(patsubst src/%.c,build/%.o,$(SRC))
BIN     := nsh

.PHONY: all test valgrind bench clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

test: $(BIN)
	./tests/run.sh

valgrind: $(BIN)
	NSH_WRAPPER="valgrind --leak-check=full --error-exitcode=99 -q" ./tests/run.sh

bench: $(BIN)
	./bench/run_all.sh

clean:
	rm -rf build $(BIN)
