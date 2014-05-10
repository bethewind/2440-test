touch.bin : head.o init2440.o s2440lib.o main.o nand.o lcd.o
	arm-linux-ld -Ttouch.lds $^ -o touch_elf
	arm-linux-objcopy -O binary touch_elf $@
	arm-linux-objdump -D touch_elf > touch.dis
	
%.o : %.c
	arm-linux-gcc -nostdlib -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -nostdlib -Wall -c -O2 -o $@ $<
clean:
	rm -f  touch.dis touch.bin touch_elf *.o
