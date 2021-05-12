Version = 0.1
Install_prefix = /usr/local
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC

OBJCC = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))

LDFLAGS = -lm

all: obj libpgm.so

libpgm.so: $(OBJ)
	$(CC) $(CFLAGS) -shared $(OBJ) $(LDFLAGS) -o libpgm.so

obj/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

obj:
	mkdir -p obj


install: uninstall
	cp include/libpgm.h $(Install_prefix)/include

	cp libpgm.so $(Install_prefix)/lib/libpgm.so.$(Version)
	@cd $(Install_prefix)/lib/ ; ln -s libpgm.so.$(Version) libpgm.so

	@rm -f libpgm.pc
	@echo "Name: libpgm" >> libpgm.pc
	@echo "Description: github.com/uzunenes/libpgm" >> libpgm.pc
	@echo "Version: $(Version)" >> libpgm.pc
	@echo "Libs: -L$(Install_prefix)/lib -lpgm" >> libpgm.pc
	@echo "Cflags: -I$(Install_prefix)/include" >> libpgm.pc

	cp libpgm.pc $(Install_prefix)/lib/pkgconfig


uninstall:
	rm -f /usr/local/include/libpgm.h
	rm -f /usr/local/lib/libpgm.so*
	rm -f /usr/local/lib/pkgconfig/libpgm.pc


clean:
	rm -f *.o obj/*.o *.so *.pc
