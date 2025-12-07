CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lpigiem -lm

all: gaussian_blur.out

gaussian_blur.out: gaussian_blur.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.out *.pgm

.PHONY: all clean
