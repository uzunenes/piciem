CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lpigiem -lm

all: denoise_median.out

denoise_median.out: denoise_median.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.out *.pgm

.PHONY: all clean
