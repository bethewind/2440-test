watchdog.bin : head.o init2440.o s2440lib.o main.o
	arm-linux-ld -Ttext 0x00000000 $^ -o watchdog_elf
	arm-linux-objcopy -O binary watchdog_elf $@
	arm-linux-objdump -D watchdog_elf > watchdog.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  watchdog.dis watchdog.bin watchdog_elf *.o
