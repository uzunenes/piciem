# Thresholding Examples Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpigiem -lm

all: simple_threshold.out otsu_threshold.out

simple_threshold.out: simple_threshold.c
	$(CC) $(CFLAGS) -o simple_threshold.out simple_threshold.c $(LDFLAGS)

otsu_threshold.out: otsu_threshold.c
	$(CC) $(CFLAGS) -o otsu_threshold.out otsu_threshold.c $(LDFLAGS)

clean:
	rm -f *.out *.o output_*.pgm
