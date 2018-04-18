# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -std=c++14 -pthread
LINKS = -lm

TARGETS = pi pi_v1 pi_v2 pi_v3 pi_v4 pi_v5

all: run

run: 
	for t in $(TARGETS) ; do \
		$(CC) $(CFLAGS) -o $$t $$t.cpp $(LINKS) ; \
	done

clean:
	for t in $(TARGETS) ; do \
		rm -f t ; \
	done