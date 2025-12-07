# Frequency Domain Examples Makefile

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpigiem -lm

all: dft_example.out fft_example.out

dft_example.out: dft_example.c
	$(CC) $(CFLAGS) -o dft_example.out dft_example.c $(LDFLAGS)

fft_example.out: fft_example.c
	$(CC) $(CFLAGS) -o fft_example.out fft_example.c $(LDFLAGS)

clean:
	rm -f *.out *.o output_*.pgm
