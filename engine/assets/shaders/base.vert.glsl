#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;
out vec3 LightDir;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uLightPos;
uniform vec3 uLightDir;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
    Normal = mat3(transpose(inverse(uView * uModel))) * aNorm;  // Move out transpose inverse matrix calculation to render loop.
    FragPos = vec3(uView * uModel * vec4(aPos, 1.0f));
    TexCoord = aTexCoord;

    LightPos = uLightPos; //vec3(uView * vec4(uLightPos, 1.0f));
    LightDir = uLightDir; //vec3(uView * vec4(uLightDir, 0.0f)); // w coord is 0.0f because we care only about direction, so we need to remove translation. Also can use mat3(uView) * uLightDir
}
