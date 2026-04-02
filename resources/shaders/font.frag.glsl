#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform float uSmoothing;

void main()
{
    float dist = texture2D(uTexture, TexCoord).r;
    float alpha = smoothstep(0.5 - uSmoothing, 0.5 + uSmoothing, dist);

    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
