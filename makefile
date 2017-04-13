src=$(wildcard *.c)
obj=$(src:.c=.o)
dep=$(obj:.o=.d)
CC=gcc

LDFLAGS=$(shell pkg-config --libs libserialport menu ncurses)
CFLAGS=-Wall -Wextra -pedantic -std=c99 $(shell pkg-config --cflags libserialport menu ncurses)

nc-servoterm: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)
%d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) nc-servoterm

.PHONY: cleandep
cleandep:
	rm -f $(dep)
