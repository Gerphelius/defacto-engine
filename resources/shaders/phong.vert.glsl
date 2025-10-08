#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
    Normal = mat3(transpose(inverse(uView * uModel))) * aNorm;  // TODO: Move out transpose inverse matrix calculation to render loop.
    FragPos = vec3(uView * uModel * vec4(aPos, 1.0f));
    TexCoord = aTexCoord;
}
