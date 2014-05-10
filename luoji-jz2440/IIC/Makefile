iic.bin : head.o init2440.o s2440lib.o main.o
	arm-linux-ld -Ttext 0x00000000 $^ -o iic_elf
	arm-linux-objcopy -O binary iic_elf $@
	arm-linux-objdump -D iic_elf > iic.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  iic.dis iic.bin iic_elf *.o
