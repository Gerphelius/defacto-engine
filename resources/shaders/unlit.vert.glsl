#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0f);
    FragPos = vec3(uView * uModel * vec4(aPos, 1.0f));
    TexCoord = aTexCoord;
}
