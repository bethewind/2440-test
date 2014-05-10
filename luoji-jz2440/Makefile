led.bin : led.S
	arm-linux-gcc -c led.S -o led.o
	arm-linux-ld -Ttext 0x00000000 led.o -o led_elf
	arm-linux-objcopy -O binary -S led_elf led.bin
clean:
	rm -f led.bin led_elf *.o