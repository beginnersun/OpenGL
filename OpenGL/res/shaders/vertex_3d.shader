#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0);
}