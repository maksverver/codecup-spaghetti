# Requires GNU make and GCC to build.

CC=g++
CXX=g++
CFLAGS=-Wall -Wextra -pipe -O2 -march=native -g -DDEBUG -Wno-sign-compare
CXXFLAGS=$(CFLAGS) --std=c++17
LDLIBS=-lm
OUTPUT_OPTION=-MMD -MP -o $@

SRCS=$(wildcard *.cc)
OBJS=$(SRCS:.cc=.o)
DEPS=$(SRCS:.cc=.d)
BINS=player game_test

all: $(BINS)

player: player.o game.o random.o

game_test: game.o random.o game_test.o

test: game_test
	./game_test

clean:
	rm -f $(DEPS) $(OBJS) $(BINS)

-include $(DEPS)

.PHONY: all clean
