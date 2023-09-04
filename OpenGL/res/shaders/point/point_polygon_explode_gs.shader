
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3)out;

uniform float time;

in VS_OUT{
	vec2 texCoords;
} fs_in[];

out vec2 TexCoords;

//计算当前的法向量 仅适用于接收参数为面 eg:layout(triangle) in; 
vec3 getNormal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(b, a));
}
vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
	//延长向量长度（扩大点的位置）
	vec3 direction = normal * ((sin(time) + 1.0) / 2) * magnitude;
	return position + vec4(direction, 0.0);
}
void main() {
	vec3 normal = getNormal();
	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = fs_in[0].texCoords;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = fs_in[1].texCoords;
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = fs_in[2].texCoords;
	EmitVertex();

	EndPrimitive();
}
