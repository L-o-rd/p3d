#version 330 core

out vec4 out_Color;
in vec3 tex_Coords;

uniform samplerCube skybox;

void main() {
    vec4 col = texture(skybox, tex_Coords);
    float fog = exp(-0.00004 * 50.f * 50.f);
    fog = clamp(fog, 0.0, 1.0);
    col = fog * col + (1.0 - fog) * vec4(0.9f, 0.85f, 0.75f, 1.0f);
    out_Color = vec4(col.xyz, 1.0);
}