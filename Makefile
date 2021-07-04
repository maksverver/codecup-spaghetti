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
BINS=analyze game_test player

COMMON_OBJS = game.o io.o minimax.o random.o timing.o

all: $(BINS)

test: game_test
	./game_test

clean:
	rm -f $(DEPS) $(OBJS) $(BINS)

analyze: analyze.o $(COMMON_OBJS)

game_test: game_test.o $(COMMON_OBJS)

player: player.o $(COMMON_OBJS)

-include $(DEPS)

.PHONY: all clean
