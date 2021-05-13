Version = 0.1
Install_prefix = /usr/local
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC

OBJCC = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))

LDFLAGS = -lm

all: obj pigiem.so

pigiem.so: $(OBJ)
	$(CC) $(CFLAGS) -shared $(OBJ) $(LDFLAGS) -o pigiem.so

obj/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

obj:
	mkdir -p obj


install: uninstall
	cp include/pigiem.h $(Install_prefix)/include

	cp pigiem.so $(Install_prefix)/lib/libpigiem.so.$(Version)
	@cd $(Install_prefix)/lib/ ; ln -s libpigiem.so.$(Version) libpigiem.so

	@rm -f pigiem.pc
	@echo "Name: pigiem" >> pigiem.pc
	@echo "Description: github.com/uzunenes/pigiem" >> pigiem.pc
	@echo "Version: $(Version)" >> pigiem.pc
	@echo "Libs: -L$(Install_prefix)/lib -lpigiem" >> pigiem.pc
	@echo "Cflags: -I$(Install_prefix)/include" >> pigiem.pc

	cp pigiem.pc $(Install_prefix)/lib/pkgconfig


uninstall:
	rm -f $(Install_prefix)/include/pigiem.h
	rm -f $(Install_prefix)/lib/libpigiem.so*
	rm -f $(Install_prefix)/lib/pkgconfig/pigiem.pc


clean:
	rm -f *.o obj/*.o *.so *.pc
