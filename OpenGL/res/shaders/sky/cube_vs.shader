#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
//layout(location = 1) in vec2 aTexCoords; �Ƴ�����λ�ã����÷���sky����ȡ��ɫ

out vec2 TexCoords;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	float ratio = 1.00 / 1.52;
	Normal = mat3(transpose(inverse(model))) * aNormal;
	Position = vec3(model * vec4(aPos,ratio));
	//TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}