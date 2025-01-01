#version 330 core

layout (location = 0) in vec3 in_Pos;
out vec3 tex_Coords;

uniform mat4 projection;
uniform mat4 view;

void main() {
	vec4 pos = projection * view * vec4(in_Pos, 1.0);
	tex_Coords = vec3(in_Pos.x, in_Pos.y, -in_Pos.z);
	gl_Position = pos.xyww;
}