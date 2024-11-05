#version 460 core
out vec4 FragColor;

in vec4 v_Color;

void main() {
    if(!gl_FrontFacing) FragColor = vec4(1.0,1.0,1.0,1.0);
    else FragColor = vec4(v_Color.r/255.0, v_Color.g/255.0, v_Color.b/255.0, v_Color.a/255.0);
}
