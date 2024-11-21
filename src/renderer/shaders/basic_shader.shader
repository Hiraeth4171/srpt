#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;

uniform vec2 resolution;
uniform mat4 proj;
uniform mat4 view;

out vec4 v_Color;

void main() {
    v_Color = aCol;
    gl_Position = proj * view * vec4(aPos, 1.0);
    //gl_Position = vec4(2.0*(aPos.x/resolution.x)-1, (-2.0)*(aPos.y/resolution.y)+1.0, -0.5*aPos.z, 1.0);
}
