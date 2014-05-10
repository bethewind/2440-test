ledkey.bin : init.S main.c
	arm-linux-gcc -c -g init.S -o init.o
	arm-linux-gcc -c -g main.c -o main.o
	arm-linux-ld -Ttext 0x00000000 init.o main.o -o ledkey_elf
	arm-linux-objcopy -O binary ledkey_elf ledkey.bin
	arm-linux-objdump -D ledkey_elf > ledkey.dis
clean :
	rm -f init.o main.o ledkey_elf ledkey.bin
