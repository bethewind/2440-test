uart.bin : head.o init2440.o s2440lib.o main.o
	arm-linux-ld -Ttext 0x00000000 $^ -o uart_elf
	arm-linux-objcopy -O binary uart_elf $@
	arm-linux-objdump -D uart_elf > uart.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  uart.dis uart.bin uart_elf *.o
