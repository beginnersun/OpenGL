#version 330 core

in vec4 outColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
	FragColor = outColor;
	FragColor = texture(ourTexture, TexCoord) * FragColor;
};