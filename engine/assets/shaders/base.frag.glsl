#version 460 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position; // Currently unused, using in LightPos, because it is in view space.

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

out vec4 FragColor;

uniform sampler2D uTexture;

uniform Material uMaterial;
uniform Light uLight;

void main()
{
    vec3 norm = normalize(Normal);

    vec3 ambient = uLight.ambient * uMaterial.ambient;

    vec3 lightDir = normalize(LightPos - FragPos);
    float angle = max(dot(norm, lightDir), 0);
    vec3 diffuse = uLight.diffuse * angle * uMaterial.diffuse;

    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uMaterial.specular * shininess * uLight.specular;

    FragColor = vec4((diffuse + ambient + specular), 1.0);
}
