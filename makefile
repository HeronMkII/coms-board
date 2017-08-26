CC = avr-gcc
CFLAGS = -g -mmcu=atmega32m1 -Os -mcall-prologues
PROG = stk500# the Pololu acts like the STK500
MCU = m32m1
PORT = /dev/tty.usbmodem00187462

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

coms: $(OBJ)
	$(CC) $(CFLAGS) -o ./build/$@.elf $^
	avr-objcopy -j .text -j .data -O ihex ./build/$@.elf ./build/$@.hex

-include $(DEP)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean upload

clean:
	rm -f $(OBJ)
	rm -f $(DEP)
	rm -f ./build/*

upload: coms
	avrdude -p $(MCU) -c $(PROG) -P $(PORT) -U flash:w:./build/$^.hex
