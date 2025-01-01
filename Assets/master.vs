#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec3 in_Color;

out vec4 gl_Position;
out vec4 ex_Color;
out vec3 Normal;
out vec3 FragPos;
out vec3 inLightPos;
out vec3 inViewPos;
out vec3 WorldPos;
out vec4 FLights[4];
out vec4 FWorlds[4];

uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 lights;
uniform mat4 model;
uniform mat4 view;
uniform int fly;

void main(void) {
    gl_Position = projection * view * model * vec4(in_Position, 1.0);
    Normal = vec3(projection * view * vec4(in_Normal, 0.0));
    inLightPos= vec3(projection*view* vec4(lightPos, 1.0f));
    inViewPos=vec3(projection*view*vec4(viewPos, 1.0f));
    ex_Color = vec4(in_Color.xyz, 1);
    WorldPos = vec3(model * vec4(in_Position, 1.0));
    FragPos = vec3(gl_Position);

    for (int i = 0; i < 4; ++i) {
        FLights[i] = projection * view * model * lights[i];
        FWorlds[i] = lights[i];
    }
}