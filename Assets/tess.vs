#version 410 core

layout(location = 0) in vec2 in_Pos;
out vec2 tex_Coord;

void main() {
    tex_Coord = in_Pos;
    gl_Position = vec4(in_Pos, 0.0, 1.0);
}