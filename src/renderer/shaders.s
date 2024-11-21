.section .rodata
    // vertex shader
    .global vshader
    .type vshader, @object
    .align 4
vshader:
    .incbin "./shaders/basic_shader.shader"
vshader_end:
    .global vshader_size
    .type vshader_size, @object
    .align 4
vshader_size:
    .int vshader_end - vshader


    // frag shader
    .global fshader
    .type fshader, @object
    .align 4
fshader:
    .incbin "./shaders/basic_frag_shader.shader"
fshader_end:
    .global fshader_size
    .type fshader_size, @object
    .align 4
fshader_size:
    .int fshader_end - fshader
