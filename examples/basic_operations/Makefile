# Basic Operations Examples Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpigiem -lm

all: brightness_contrast.out invert.out

brightness_contrast.out: brightness_contrast.c
	$(CC) $(CFLAGS) -o brightness_contrast.out brightness_contrast.c $(LDFLAGS)

invert.out: invert.c
	$(CC) $(CFLAGS) -o invert.out invert.c $(LDFLAGS)

clean:
	rm -f *.out *.o output_*.pgm
