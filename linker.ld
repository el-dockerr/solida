ENTRY(kernel_main)

SECTIONS
{
    /* Set the starting address to 1MB */
    . = 1M;

    /* Multiboot header should be the first part of the text section */
    .multiboot ALIGN(4):
    {
        *(.multiboot)
    }

    .text :
    {
        *(.text*)
    }

    /* Read-only data */
    .rodata :
    {
        *(.rodata*)
    }

    /* Data section */
    .data :
    {
        *(.data*)
    }

    /* BSS section */
    .bss :
    {
        *(.bss*)
    }

    /* Discard unused sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}