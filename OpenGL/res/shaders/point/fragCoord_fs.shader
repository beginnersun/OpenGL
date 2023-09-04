#version 330 core
out vec4 FragColor;
/*
in VS_OUT{
	vec2 TexCoords;
}fs_in;*/
in vec2 TexCoords;

uniform sampler2D boxTextureOut;
uniform sampler2D boxTextureIn;

void main() {
	if (gl_FragCoord.x < 500) {
		FragColor = texture(boxTextureOut, TexCoords);
	} else {
		if (gl_FrontFacing) {
			FragColor = texture(boxTextureOut, TexCoords);
		}
		else {
			FragColor = texture(boxTextureIn, TexCoords);
		}
	}
}