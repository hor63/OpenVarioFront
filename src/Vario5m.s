
/*
 * Include the file "Vario5m.png" from the same directory as this assembler
 * source.
 * The start of the
*/

/* Make the GNU chain happy */
.section .note.GNU-stack,"",@progbits

    .section .rodata

    .global Vario5m
    .type   Vario5m, @object
    .align  4
Vario5m:
    .incbin "Vario5m.png"
Vario5m_end:
    .global Vario5m_size
    .type   Vario5m_size, @object
    .align  4
Vario5m_size:
    .int    Vario5m_end - Vario5m
