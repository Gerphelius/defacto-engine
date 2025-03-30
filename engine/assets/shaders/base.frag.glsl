#version 460 core

in vec3 vertexColor;
in vec2 TexCoord;
out vec3 Pos;

out vec4 FragColor;

uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform float uTime;

void main()
{
    FragColor = texture(uTexture1, TexCoord) - texture(uTexture2, TexCoord) * vec4(vertexColor, 1.0f);
}
