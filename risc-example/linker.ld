MEMORY {
    FLASH   (rx)    :   ORIGIN = 0x00000000, LENGTH = 1M
    RAM     (rwx)   :   ORIGIN = 0x10000000, LENGTH = 2M
}

ENTRY(_start);

SECTIONS {
    .crt0 : {

    } >FLASH

    .text : {
        *(.text)
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))
    } >RAM

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >RAM

    .preinit_array : {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >RAM

    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >RAM

    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >RAM

    .data : {
        . = ALIGN(4);
        _data_begin_ = .;        
        *(.data)       
        *(.data*)        

        . = ALIGN(4);
        _data_end_ = .;   
    } >RAM

    .bss : {
        . = ALIGN(4);
        _bss_start_ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end_ = .;
    } >RAM

    /DISCARD/ : {
        *(.ARM.*)
    }

}