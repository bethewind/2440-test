SECTIONS {
    first 0x00000000 : {head.o init2440.o nand.o}
    second 0x30000000 : AT(4096) {main.o s2440lib.o lcd.o pic.o}
}