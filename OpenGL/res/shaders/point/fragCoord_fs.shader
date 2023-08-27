#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D boxTextureOut;
uniform sampler2D boxTextureIn;

void main() {
	/*if (gl_FragCoord.x < 400.0) {
		FragColor = texture(boxTextureOut, TexCoords);
	} else {*/
		if (gl_FrontFacing) {
			FragColor = texture(boxTextureOut, TexCoords);
		}
		else {
			FragColor = texture(boxTextureIn, TexCoords);
		}
	//}
}