#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos; //片段着色器在计算漫反射光照时需要有一个顶点位置（在世界空间中的，所以还需要进行换算  乘以模型矩阵）
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

	FragPos = vec3(model * vec4(aPos, 1.0));

	Normal = mat3(transpose(inverse(model))) * aNormal;

	TexCoords = aTexCoords;

	gl_Position = projection * view * model * vec4(aPos, 1.0);

}