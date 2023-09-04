/*#version 330 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;
//�������ṩ���ڽ�GL����
in gl_Vertex
{
	vec4  gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];  //�����ýӿڿ�Ϊ��������Ϊ ������ɫ�����͵����ݵ�������ɫ����һ��һ�鷢�͵ģ��磬�ߡ������εȵȣ�
//���Ծ����ǻ��Ƶ�ҲҪʹ��[]�����ʽ����
//���Ҫʹ�� in vec3 color[] ���ָ�ʽ Ҳ����

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

//���㵱ǰ�ķ����� �������ڽ��ղ���Ϊ�� eg:layout(triangle) in; 
/*vec3 getNormal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}
vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
	//�ӳ��������ȣ�������λ�ã�
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
