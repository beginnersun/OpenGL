#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in NORMAL_VS_OUT{
	vec3 normal;
}vs_in[];

uniform mat4 projection;

const float MAGNITUDE = 0.2;

void generateNormalLine(int index) {
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();

	gl_Position = projection * (gl_in[index].gl_Position + vec4(vs_in[index].normal, 0.0) * MAGNITUDE);
	EmitVertex();
	EndPrimitive();
}

void main() {
	generateNormalLine(0);
	generateNormalLine(1);
	generateNormalLine(2);
}
