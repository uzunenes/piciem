Version = 0.1


all: pgm_io.o image.o
	gcc -shared -o libpgm.so pgm_io.o image.o

pgm_io.o: src/pgm_io.c
	gcc -O3 -Wall -fpic -c src/pgm_io.c

image.o: src/image.c
	gcc -O3 -Wall -fpic -c src/image.c


install:
	@rm -f /usr/local/include/libpgm
	cp include/libpgm.h /usr/local/include

	@rm -f /usr/local/lib/libpgm.so*
	cp libpgm.so /usr/local/lib/libpgm.so.$(Version)
	@cd /usr/local/lib/ ; ln -s libpgm.so.$(Version) libpgm.so

	@echo "Name: libpgm" >> libpgm.pc
	@echo "Description: github.com/uzunenes/libpgm" >> libpgm.pc
	@echo "Version: $(Version)" >> libpgm.pc
	@echo "Libs: -L/usr/local/lib -lpgm" >> libpgm.pc
	@echo "Cflags: -I/usr/local/include/libpgm" >> libpgm.pc

	@cp libpgm.pc /usr/local/lib/pkgconfig


clean:
	rm *.o *.so *.pc
