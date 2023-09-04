/*#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;
//下面是提供的内建GL变量
in gl_Vertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];  //此内置接口块为数组是因为 顶点着色器发送点数据到几何着色器是一组一组发送的（如，线、三角形等等）
//所以就算是绘制点也要使用[]数组格式接收
//如果要使用 in vec3 color[] 这种格式 也可以

void main() {
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}*/
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4)out;

uniform float time;

//计算当前的法向量 仅适用于接收参数为面 eg:layout(triangle) in; 
/*vec3 getNormal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}
vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
	//延长向量长度（扩大点的位置）
	vec3 direction = normal * ((sin(time) + 1.0) / 2) * magnitude;
	return position + vec4(direction, 0.0);
}*/
void main() {
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();
}
