lcd.bin : head.o init2440.o s2440lib.o main.o lcd.o nand.o pic.o
	arm-linux-ld -Tlcd.lds $^ -o lcd_elf
	arm-linux-objcopy -O binary lcd_elf $@
	arm-linux-objdump -D lcd_elf > lcd.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  lcd.dis lcd.bin lcd_elf *.o
