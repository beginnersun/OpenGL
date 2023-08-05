#version 330 core
out vec4 FragColor;

in vec3 LightingColor;
uniform vec3 objectColor;

void main() {
	vec3 result = LightingColor * objectColor;
	FragColor = vec4(result, 1.0f);
}
