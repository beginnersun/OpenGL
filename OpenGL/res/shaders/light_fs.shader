#version 330 core
out vec4 FragColor;

//对应的平面的法向量
in vec3 Normal;
//片段着色器中的当前顶点位置（世界空间中的）
in vec3 FragPos;

//根据光源的位置 与当前的片段向量  计算漫反射光照的颜色
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main() {
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	//normalize方法是为了将向量标准化 化为单位向量  因为在这里面，我们不关心向量的长度与位置
	vec3 norm = normalize(Normal); 
	vec3 lightDir = normalize(lightPos - FragPos);
	//将法向量与光源方向点乘 计算光源对漫反射的影响 a * b = |a| * |b| * cosθ  因为ab都是单位向量所以|a|与|b|都是1
	float diff = max(dot(norm, lightDir), 0.0);
	//然后乘以光的颜色  最终得到漫反射后的颜色
	vec3 diffuse = diff * lightColor;

	//镜反射光照s
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos); //计算视觉向量

	vec3 reflectDir = reflect(-lightDir,norm); //计算光反射向量  -lightDir 是光源指向片段的向量  norm是法向量  最后得到反射向量
	float spec = pow(max(dot(viewDir,reflectDir),0.0),256);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	//FragColor = vec4(lightColor * objectColor, 1.0f);
	FragColor = vec4(result, 1.0f);

}
