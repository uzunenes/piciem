example_name=pgm_read_chance_block_write


all: $(example_name).out

$(example_name).out: $(example_name).o
	gcc -Wall -o $(example_name).out $(example_name).o -lpgm

$(example_name).o: $(example_name).c
	gcc -Wall -o $(example_name).o -c $(example_name).c


clean:
	rm *.o *.out