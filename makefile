src = $(wildcard *.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)

LDFLAGS = -lncurses -lmenu -lpthread -lserialport

main: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)
%d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) main

.PHONY: cleandep
cleandep:
	rm -f $(dep)
