#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec4 outColor;
out vec2 TexCoord;

uniform vec3 p_offset;
uniform mat4 transform;

void main()
{
	gl_Position = gl_Position = transform * vec4(position.x + p_offset.x, position.y + p_offset.y, position.z, position.w);
	
	outColor = aColor;
	TexCoord = aTexCoord;
}

#shader fragment
#version 330 core

in vec4 outColor;
out vec4 FragColor;
in vec2 TexCoord;

uniform vec4 ourColor;
uniform float mixValue;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
	vec4 centerColor = vec4(ourColor.x+outColor.x,ourColor.y+outColor.y,ourColor.z+outColor.z,1.0);
	FragColor = mix(texture(ourTexture1,TexCoord),texture(ourTexture2,vec2(TexCoord.x,TexCoord.y)),mixValue);
};