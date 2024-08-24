#version 460 core
out vec4 FragColor;

uniform vec4 backgroundColor;

void main() {
    if(!gl_FrontFacing) FragColor = vec4(1.0,1.0,1.0,1.0);
    else FragColor = vec4(backgroundColor.r/255.0, backgroundColor.g/255.0, backgroundColor.b/255.0, backgroundColor.a/255.0);
}
