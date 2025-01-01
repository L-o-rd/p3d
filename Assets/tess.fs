#version 410 core

in vec4 frag_Color;
in vec3 FragPos;
in vec3 Normal;
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 WorldPos;
in vec4 FLights[4];
in vec4 FWorlds[4];

out vec4 out_Color;

vec3 apply_light(in vec3 emission, in vec3 lightColor, in vec3 lightPos) {
    vec3 objectColor = frag_Color.xyz;
    float ambientStrength = 0.5f;
    vec3 ambient_light = ambientStrength * lightColor;
    vec3 ambient_term = ambient_light * objectColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse_light = lightColor;
    vec3 diffuse_term = diff * diffuse_light * objectColor;

    float specularStrength = 0.5f;
    float shininess = 10.0f;
    vec3 viewDir = normalize(inViewPos - FragPos); 
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular_light = specularStrength  * lightColor; 
    vec3 specular_term = spec * specular_light * objectColor;

    return emission + (ambient_term + diffuse_term + specular_term);
}

void main() {
    vec3 lightColor = vec3(0.45f, 0.325f, 0.265f);
    vec3 objectColor = vec3(frag_Color);
    float ambientStrength = 0.4f;
    vec3 ambient_light = ambientStrength * lightColor;
    vec3 ambient_term = ambient_light * objectColor;
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(inLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse_light = lightColor;
    vec3 diffuse_term = diff * diffuse_light * objectColor;
    
    float specularStrength = 0.5f;
    float shininess = 2.0f;
    vec3 viewDir = normalize(inViewPos - FragPos); 
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular_light = specularStrength  * lightColor; 
    vec3 specular_term = spec * specular_light * objectColor;
    
    vec3 emission=vec3(0.0, 0.0, 0.0);
    vec3 result = emission + (ambient_term + diffuse_term + specular_term);

    float dist1 = length(WorldPos - FWorlds[0].xyz);
    float dist2 = length(WorldPos - FWorlds[1].xyz);
    float dist3 = length(WorldPos - FWorlds[2].xyz);
    float dist4 = length(WorldPos - FWorlds[3].xyz);
    dist1 = 2.0 / (0.1 + dist1 * 1.0 + dist1 * dist1 * 0.5);
    dist2 = 2.0 / (0.1 + dist2 * 1.0 + dist2 * dist2 * 0.5);
    dist3 = 2.0 / (0.1 + dist3 * 1.0 + dist3 * dist3 * 0.5);
    dist4 = 2.0 / (0.1 + dist4 * 1.0 + dist4 * dist4 * 0.5);
    result += apply_light(vec3(0.9f, 0.9f, 0.1f), vec3(0.9f, 0.9f, 0.1f), FLights[0].xyz) * dist1;
    result += apply_light(vec3(0.9f, 0.9f, 0.1f), vec3(0.9f, 0.9f, 0.1f), FLights[1].xyz) * dist2;
    result += apply_light(vec3(0.9f, 0.9f, 0.1f), vec3(0.9f, 0.9f, 0.1f), FLights[2].xyz) * dist3;
    result += apply_light(vec3(0.9f, 0.9f, 0.1f), vec3(0.9f, 0.9f, 0.1f), FLights[3].xyz) * dist4;

    float fog = exp(-0.000013 * FragPos.z * FragPos.z);
    fog = clamp(fog, 0.0, 1.0);
    result = fog * result + (1.0 - fog) * vec3(0.9f, 0.85f, 0.75f);
    out_Color = vec4(result, 1);
}