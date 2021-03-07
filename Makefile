Version = 0.1
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))


all: obj libpgm.so

libpgm.so: $(OBJ)
	$(CC) $(CFLAGS) -shared $(OBJ) -o libpgm.so

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj


install:
	@rm -f /usr/local/include/libpgm.h
	cp include/libpgm.h /usr/local/include

	@rm -f /usr/local/lib/libpgm.so*
	cp libpgm.so /usr/local/lib/libpgm.so.$(Version)
	@cd /usr/local/lib/ ; ln -s libpgm.so.$(Version) libpgm.so

	@rm -f /usr/local/lib/pkgconfig/libpgm.pc
	@echo "Name: libpgm" >> libpgm.pc
	@echo "Description: github.com/uzunenes/libpgm" >> libpgm.pc
	@echo "Version: $(Version)" >> libpgm.pc
	@echo "Libs: -L/usr/local/lib -lpgm" >> libpgm.pc
	@echo "Cflags: -I/usr/local/include" >> libpgm.pc

	@cp libpgm.pc /usr/local/lib/pkgconfig


clean:
	rm -f *.o obj/*.o *.so *.pc
