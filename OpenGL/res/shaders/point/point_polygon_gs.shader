/*#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;
//下面是提供的内建GL变量
in gl_Vertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];

void main() {
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}*/
#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 3)out;
void main() {
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.1, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}