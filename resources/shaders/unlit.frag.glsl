#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D emmisive;
};

in vec2 TexCoord;
in vec3 FragPos;

out vec4 FragColor;

uniform Material uMaterial;

void main()
{
    vec4 emmisive = texture(uMaterial.emmisive, TexCoord);

    FragColor = texture(uMaterial.diffuse, TexCoord) + emmisive;
}
