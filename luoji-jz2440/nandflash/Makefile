nand.bin : head.o init2440.o s2440lib.o main.o nand.o
	arm-linux-ld -Tnand.lds $^ -o nand_elf
	arm-linux-objcopy -O binary nand_elf $@
	arm-linux-objdump -D nand_elf > nand.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  nand.dis nand.bin nand_elf *.o
