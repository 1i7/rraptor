TARGET		= $(CURDIR)
#OS		= $(shell cd /Users/tomilovandrej/Desktop/uos-embedded-read-only; pwd)
OS		= $(shell cd ../../local/uos-embedded-read-only; pwd)
include target.cfg

CFLAGS          += -g
PROGTOOL        = $(AVRDUDE) -c stk500v2 -p pic32 -b 115200

TEST		= rraptor_init
TESTS		= $(TEST).srec

OUTS		= $(TESTS:%.srec=%.elf)

all:		$(OUTS) $(TESTS) $(TEST).dis
		$(SIZE) *.elf

$(OUTS) $(TESTS): startup.o libuos.a

clean:
		rm -rf *~ *.[oasi] *.elf *.hex *.srec *.bin *.lst *.dis .deps $(MODULES)

hex:		$(TESTS)
		for srec in $(TESTS); do \
			$(OBJCOPY) -O ihex -I srec $$srec `basename $$srec .srec`.hex; \
		done

load:		$(TEST).srec
		$(OBJCOPY) -O ihex $(TEST).srec $(TEST).hex
		$(PROGTOOL) -v -U flash:w:$(TEST).hex:i

debug:		$(TEST).srec
		$(GDB) $(TEST).elf

include $(OS)/sources/rules.mak

###
