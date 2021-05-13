Version = 0.1
Prefix = /usr/local
CC = gcc
CFLAGS = -Wall -Wextra -O3 -fPIC

OBJCC = $(patsubst src/%.c, obj/%.o, $(wildcard src/*c))

LDFLAGS = -lm

all: obj pigiem.so

pigiem.so: $(OBJCC)
	$(CC) $(CFLAGS) -shared $(OBJCC) $(LDFLAGS) -o pigiem.so

obj/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

obj:
	mkdir -p obj


install: uninstall
	cp include/pigiem.h $(Prefix)/include

	cp pigiem.so $(Prefix)/lib/libpigiem.so.$(Version)
	@cd $(Prefix)/lib/ ; ln -s libpigiem.so.$(Version) libpigiem.so

	@rm -f libpigiem.pc
	@echo "Name: libpigiem" >> libpigiem.pc
	@echo "Description: github.com/uzunenes/pigiem" >> libpigiem.pc
	@echo "Version: $(Version)" >> libpigiem.pc
	@echo "Libs: -L$(Prefix)/lib -lpigiem" >> libpigiem.pc
	@echo "Cflags: -I$(Prefix)/include" >> libpigiem.pc

	cp libpigiem.pc $(Prefix)/lib/pkgconfig


uninstall:
	rm -f $(Prefix)/include/pigiem.h
	rm -f $(Prefix)/lib/libpigiem.so*
	rm -f $(Prefix)/lib/pkgconfig/libpigiem.pc


clean:
	rm -f *.o obj/*.o *.so *.pc
