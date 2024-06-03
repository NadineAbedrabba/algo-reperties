# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Targets
TARGETS = parent child

# Rules
all: $(TARGETS)

parent: parent.c common.h
	$(CC) $(CFLAGS) -o $@ parent.c -lrt

child: child.c common.h
	$(CC) $(CFLAGS) -o $@ child.c -lrt

clean:
	rm -f $(TARGETS)

.PHONY: all clean

