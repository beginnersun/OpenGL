#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 LightingColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	vec3 FragPos = vec3(model * vec4(aPos, 1.0));

	vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

	//计算环境光照
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	//计算漫反射光照
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//计算镜反射光照
	float specularStrength = 1.0;
	vec3 viewDir = normalize(viewPos - FragPos);
	//求出反射的光的向量
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	LightingColor = (ambient + diffuse + specular);
}