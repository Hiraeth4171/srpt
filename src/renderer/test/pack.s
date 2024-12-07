
.section .rodata
    // vertex shader
    .global sdom
    .type sdom, @object
    .align 4
sdom:
    .incbin "../../../bin/test.sdom"
sdom_end:
    .global sdom_size
    .type sdom_size, @object
    .align 4
sdom_size:
    .int sdom_end - sdom
