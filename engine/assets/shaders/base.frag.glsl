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

    float constant;
    float linear;
    float quadratic;

    float innerCone;
    float outerCone;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
in vec3 LightDir;

out vec4 FragColor;

uniform Material uMaterial;
uniform Light uLight;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 tex = vec3(texture(uMaterial.diffuse, TexCoord));

    vec3 ambient = uLight.ambient * tex;

//    vec3 lightDir = normalize(-LightDir);               // Directional light
//    vec3 lightDir = normalize(LightPos - FragPos);   // Point light
//    float angle = max(dot(norm, lightDir), 0.0f);
//    vec3 diffuse = uLight.diffuse * angle * tex;


    /****** SPOTLIGHT ******/
    vec3 spotDir = normalize(-LightDir);
    vec3 lightDir = normalize(LightPos - FragPos);
    float angle = max(dot(spotDir, lightDir), 0.0f);
    float intensity = smoothstep(0.0f, 1.0f, (angle - uLight.outerCone) / (uLight.innerCone - uLight.outerCone));
    vec3 diffuse = uLight.diffuse * intensity * tex;

    vec4 specTex = texture(uMaterial.specular, TexCoord);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = pow(max(dot(normalize(-FragPos), reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = vec3(specTex) * shininess * uLight.specular;

    vec4 emmisive = (1 - ceil(specTex)) * texture(uMaterial.emmisive, TexCoord);

    float dist = distance(FragPos, LightPos);
    float attenuation = 1.0f / (uLight.constant + uLight.linear * dist + uLight.quadratic * (dist * dist));

    FragColor = vec4((diffuse + ambient + specular) * attenuation, 1.0)/* + emmisive*/;

//    FragColor = vec4(ceil(uLight.phi - angle), 0.0f, 0.0f, 1.0f);
}
