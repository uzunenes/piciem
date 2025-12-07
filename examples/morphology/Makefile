CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lpigiem -lm

all: morphology_example.out

morphology_example.out: morphology_example.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.out *.pgm

.PHONY: all clean
