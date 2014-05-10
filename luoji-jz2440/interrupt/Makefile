int.bin : head.o init.o main.o init2440.o interupt.o
	arm-linux-ld -Ttext 0x00000000 $^ -o int_elf
	arm-linux-objcopy -O binary int_elf $@
	arm-linux-objdump -D int_elf > int.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  int.dis int.bin int_elf *.o
