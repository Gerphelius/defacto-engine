#version 460 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emmisive;
    float     shininess;
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

uniform Material uMaterial;
uniform Light uLight;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 tex = vec3(texture(uMaterial.diffuse, TexCoord));

    vec3 ambient = uLight.ambient * tex;

    vec3 lightDir = normalize(LightPos - FragPos);
    float angle = max(dot(norm, lightDir), 0);
    vec3 diffuse = uLight.diffuse * angle * tex;

    vec4 specTex = texture(uMaterial.specular, TexCoord);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = vec3(specTex) * shininess * uLight.specular;

    vec4 emmisive = (1 - ceil(specTex)) * texture(uMaterial.emmisive, TexCoord);

    FragColor = vec4((diffuse + ambient + specular), 1.0) + emmisive;
}
