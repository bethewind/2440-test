dma.bin : head.o init2440.o s2440lib.o main.o nand.o
	arm-linux-ld -Tdma.lds $^ -o dma_elf
	arm-linux-objcopy -O binary dma_elf $@
	arm-linux-objdump -D dma_elf > dma.dis
	
%.o : %.c
	arm-linux-gcc -Wall -c -O2 $< -o $@
	
%.o:%.S
	arm-linux-gcc -Wall -c -O2 -o $@ $<
clean:
	rm -f  dma.dis dma.bin dma_elf *.o
