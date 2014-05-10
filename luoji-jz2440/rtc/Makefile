
CC      = arm-linux-gcc
LD      = arm-linux-ld
AR      = arm-linux-ar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

INCLUDEDIR 	:= $(shell pwd)/include
CFLAGS 		:= -Wall -O2
CPPFLAGS   	:= -nostdinc -fno-builtin -I$(INCLUDEDIR)

export 	CC LD AR OBJCOPY OBJDUMP INCLUDEDIR CFLAGS CPPFLAGS

objs := head.o init2440.o nand.o  main.o s2440lib.o lib/libc.a

rtc.bin: $(objs)
	${LD} -Trtc.lds -o rtc_elf $^
	${OBJCOPY} -O binary -S rtc_elf $@
	${OBJDUMP} -D -m arm rtc_elf > rtc.dis

.PHONY : lib/libc.a
lib/libc.a:
	cd lib; make; cd ..
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	make  clean -C lib
	rm -f rtc.bin rtc_elf rtc.dis *.o
	
