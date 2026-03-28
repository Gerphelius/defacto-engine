#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main()
{
    vec4 texture = texture2D(uTexture, TexCoord);
    FragColor = uColor + texture;
}
