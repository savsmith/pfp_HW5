# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS = -std=c++14 -g
LINKS = -lm
DEPS = graph.h
TARGETS = graph serialBellmanFord parallelBellmanFord

all: graph \
	 serial \
	 parallel

graph: graph.cpp graph.h
	icc graph.cpp -O3 -std=c++11 -g -c -o graph.o -I/opt/intel/vtune_amplifier/include

serial: graph.h graph.cpp serialBellmanFord.cpp
	icc graph.o serialBellmanFord.cpp /opt/intel/vtune_amplifier/lib64/libittnotify.a -O3 -std=c++11 -g -pthread -o serialBF -lpthread -I/opt/intel/vtune_amplifier/include

## rules for compiling .c to .o
parallel: graph.h graph.cpp parallelBellmanFord.cpp
	icc graph.o parallelBellmanFord.cpp /opt/intel/vtune_amplifier/lib64/libittnotify.a -O3 -std=c++11 -g -pthread -o parallelBF -lpthread -I/opt/intel/vtune_amplifier/include

clean:
	for t in $(TARGETS) ; do \
		rm -f t ; \
	done
	rm -f graph.o
	rm -r r*/
	rm -f pr_*.txt

