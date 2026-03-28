#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main()
{
    /// TODO: review smoothing calculation. Should be based on
    /// atlas.distanceRange and relative font scale

    float smoothing = 0.5;
    float dist = texture2D(uTexture, TexCoord).r;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
    FragColor = vec4(uColor.rgb, uColor.a * alpha);
}
