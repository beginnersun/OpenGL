#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos; //片段着色器在计算漫反射光照时需要有一个顶点位置（在世界空间中的，所以还需要进行换算  乘以模型矩阵）

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = mat3(transpose(inverse(model))) * aNormal;
}