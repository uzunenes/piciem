Version = 0.1
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))


all: obj libpgm.so

libpgm.so: $(OBJ)
	$(CC) $(CFLAGS) -shared $(OBJ) -lm -o libpgm.so

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj


install: uninstall
	cp include/libpgm.h /usr/local/include

	cp libpgm.so /usr/local/lib/libpgm.so.$(Version)
	@cd /usr/local/lib/ ; ln -s libpgm.so.$(Version) libpgm.so

	@rm -f libpgm.pc
	@echo "Name: libpgm" >> libpgm.pc
	@echo "Description: github.com/uzunenes/libpgm" >> libpgm.pc
	@echo "Version: $(Version)" >> libpgm.pc
	@echo "Libs: -L/usr/local/lib -lpgm" >> libpgm.pc
	@echo "Cflags: -I/usr/local/include" >> libpgm.pc

	cp libpgm.pc /usr/local/lib/pkgconfig


uninstall:
	rm -f /usr/local/include/libpgm.h
	rm -f /usr/local/lib/libpgm.so*
	rm -f /usr/local/lib/pkgconfig/libpgm.pc


clean:
	rm -f *.o obj/*.o *.so *.pc
