example_name=pgm_read_apply_homomorphic_filter_and_write


all: $(example_name).out

$(example_name).out: $(example_name).o
	gcc -Wall -o $(example_name).out $(example_name).o -lpigiem -lm

$(example_name).o: $(example_name).c
	gcc -Wall -o $(example_name).o -c $(example_name).c


clean:
	rm -f *.o *.out