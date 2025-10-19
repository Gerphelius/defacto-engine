#version 460 core

struct PointLight
{
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;

    float padding;
};

layout(binding = 2, std430) readonly buffer pointLightsSSBO
{
    PointLight pointLights[];
};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emmisive;
    float     shininess;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform Material uMaterial;
uniform int uPointLightsNum;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 tex = texture(uMaterial.diffuse, TexCoord).rgb;

    vec3 result = vec3(0.0);

    for (int i = 0; i < uPointLightsNum; ++i)
    {
        PointLight light = pointLights[i];

        vec3 ambient = light.ambient.rgb * tex;

        vec3 lightDir = normalize(light.position.xyz - FragPos);
        float angle = max(dot(norm, lightDir), 0.0f);
        vec3 diffuse = angle * light.diffuse.rgb * tex;

        float dist = distance(FragPos, light.position.xyz);
        float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

        vec3 specTex = texture(uMaterial.specular, TexCoord).rgb;
        vec3 reflectDir = reflect(-lightDir, norm);
        float shininess = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), uMaterial.shininess);
        vec3 specular = specTex * shininess * light.specular.rgb;

        result += (ambient + diffuse + specular) * attenuation;
    }

    vec4 specTex = texture(uMaterial.specular, TexCoord);
    //vec4 emmisive = (1 - ceil(specTex)) * texture(uMaterial.emmisive, TexCoord);
    //                ^^^^^^^^^^^^^^^^^^^  this is used to draw emissive texture only on black portion
    //                                     of specular texture (discards metal frame from container texture)
    vec4 emmisive = texture(uMaterial.emmisive, TexCoord);

    FragColor = vec4(result, 1.0)/* + emmisive*/;
}
