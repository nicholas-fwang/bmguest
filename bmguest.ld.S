OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
TARGET(binary)
ENTRY(_start)

MEMORY {
    RAM (rwx) : ORIGIN = 0x70000000, LENGTH = 128M
}

C_STACK_SIZE   = 512;
IRQ_STACK_SIZE = 512;
FIQ_STACK_SIZE = 512;
SVC_STACK_SIZE = 512;
ABT_STACK_SIZE = 512;
UND_STACK_SIZE = 512;
HEAP_SIZE  = 0x00100000;

SECTIONS
{
    .text : {
        *(.text)
    } > RAM
    .= ALIGN(4);

    .rodata : {
        *(.rodata)
        . = ALIGN(4);
    } > RAM

    .data : {
        *(.data)
        . = ALIGN(4);
    } > RAM

    .bss : {
        __bss_start__ = . ;
        *(.bss)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    .heap : {
        __begin_heap = .;
        *(.heap)
            . = . + HEAP_SIZE;
        __end_heap = .;
    } > RAM

    .stack : {
        __stack_start__ = . ;

        . += IRQ_STACK_SIZE;
        . = ALIGN (4);
        __irq_stack_top__ = . ;

        . += FIQ_STACK_SIZE;
        . = ALIGN (4);
        __fiq_stack_top__ = . ;

        . += SVC_STACK_SIZE;
        . = ALIGN (4);
        __svc_stack_top__ = . ;

        . += ABT_STACK_SIZE;
        . = ALIGN (4);
        __abt_stack_top__ = . ;

        . += UND_STACK_SIZE;
        . = ALIGN (4);
        __und_stack_top__ = . ;

        . += C_STACK_SIZE;
        . = ALIGN (4);
        __c_stack_top__ = . ;

        __stack_end__ = .;
    } >RAM
}
