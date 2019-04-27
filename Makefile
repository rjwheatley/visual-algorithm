src = $(wildcard *.c) \
      $(wildcard algorithms/*.c)
obj = $(src:.c=.o)
dep = $(obj:.o=.d)  # one dependency file for each source

LDFLAGS = -lc -lpython2.7
INCLUDES = -I/usr/include/python2.7 -I/usr/include/python2.7 -I.
CFLAGS = -std=c99 -D_GNU_SOURCE $(INCLUDES) $(/usr/bin/python2.7-config --ldflags)

alg: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)   # include all dep files in the makefile

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) alg

.PHONY: cleandep
cleandep:
	rm -f $(dep)
