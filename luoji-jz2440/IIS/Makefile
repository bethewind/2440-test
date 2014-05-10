iis.bin : head.o init2440.o s2440lib.o main.o iis.o nand.o
	arm-linux-ld -Tiis.lds $^ -o iis_elf
	arm-linux-objcopy -O binary iis_elf $@
	arm-linux-objdump -D iis_elf > iis.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  iis.dis iis.bin iis_elf *.o
