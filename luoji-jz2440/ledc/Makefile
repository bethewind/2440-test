ledc.bin : init.S main.c
	arm-linux-gcc -c -g init.S -o init.o
	arm-linux-gcc -c -g main.c -o main.o
	arm-linux-ld -Ttext 0x00000000 init.o main.o -o ledc_elf
	arm-linux-objcopy -O binary ledc_elf ledc.bin
	arm-linux-objdump -D ledc_elf > ledc.dis
clean :
	rm -f init.o main.o ledc_elf ledc.bin
