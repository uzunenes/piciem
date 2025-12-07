# Edge Detection Examples Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpigiem -lm

all: sobel.out

sobel.out: sobel.c
	$(CC) $(CFLAGS) -o sobel.out sobel.c $(LDFLAGS)

clean:
	rm -f *.out *.o output_*.pgm
