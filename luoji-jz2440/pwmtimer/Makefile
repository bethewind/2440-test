timer.bin : head.o init2440.o s2440lib.o main.o
	arm-linux-ld -Ttext 0x00000000 $^ -o timer_elf
	arm-linux-objcopy -O binary timer_elf $@
	arm-linux-objdump -D timer_elf > timer.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  timer.dis timer.bin timer_elf *.o
