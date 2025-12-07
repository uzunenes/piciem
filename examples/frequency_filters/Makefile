CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lpigiem -lm

all: lowpass_example.out

lowpass_example.out: lowpass_example.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.out *.pgm

.PHONY: all clean
