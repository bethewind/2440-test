sdram.bin : init.S main.c
	arm-linux-gcc -c -g init.S -o init.o
	arm-linux-gcc -c -g main.c -o main.o
	arm-linux-ld -Ttext 0x30000000 init.o main.o -o sdram_elf
	arm-linux-objcopy -O binary sdram_elf sdram.bin
	arm-linux-objdump -D sdram_elf > sdram.dis
clean :
	rm -f init.o main.o sdram_elf sdram.bin
