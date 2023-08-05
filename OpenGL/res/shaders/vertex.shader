#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 outColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 transform;

void main()
{
	gl_Position = position;
	outColor = aColor;
	TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(position.x,position.y,0.0f,1.0f);
}