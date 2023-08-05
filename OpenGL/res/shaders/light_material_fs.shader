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

//材质属性 定义当前材质在环境光照、漫反射、镜面光照情况下的颜色  与对应光的颜色相乘 就是我们看到该材质在阳光下的颜色
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
//对于Struct类型的uniform类型数据赋值 时name = {变量名}.{属性名} eg: setVec3("material.ambient",vec3(1.0f,1.0f,1.0f));
uniform Material material;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;

void main() {
	/*vec3 ambient = lightColor * material.ambient;
	//normalize方法是为了将向量标准化 化为单位向量  因为在这里面，我们不关心向量的长度与位置
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	//将法向量与光源方向点乘 计算光源对漫反射的影响 a * b = |a| * |b| * cosθ  因为ab都是单位向量所以|a|与|b|都是1
	float diff = max(dot(norm, lightDir), 0.0);
	//然后乘以光的颜色  最终得到漫反射后的颜色
	vec3 diffuse = lightColor * (diff * material.diffuse);

	//镜反射光照s
	vec3 viewDir = normalize(viewPos - FragPos); //计算视觉向量

	vec3 reflectDir = reflect(-lightDir, norm); //计算光反射向量  -lightDir 是光源指向片段的向量  norm是法向量  最后得到反射向量
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);*/

	// 环境光
	vec3 ambient = light.ambient * material.ambient;

	// 漫反射 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// 镜面光
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}
