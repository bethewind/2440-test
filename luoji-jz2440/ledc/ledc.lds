ENTRY(_start)
SECTIONS {
    . = 0x00000;
    .text : AT(3) { *(.text) }
}