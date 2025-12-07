# Histogram Examples Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpigiem -lm

all: histogram_eq.out

histogram_eq.out: histogram_eq.c
	$(CC) $(CFLAGS) -o histogram_eq.out histogram_eq.c $(LDFLAGS)

clean:
	rm -f *.out *.o output_*.pgm
