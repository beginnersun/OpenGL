#version 330 core
out vec4 FragColor;

//对应的平面的法向量
in vec3 Normal;
//片段着色器中的当前顶点位置（世界空间中的）
in vec3 FragPos;

in vec2 TexCoords;

//根据光源的位置 与当前的片段向量  计算漫反射光照的颜色
//uniform vec3 lightPos;   替换为结构体中的Light->position 属性
//uniform vec3 lightColor; lightColor 不再使用，而是被漫反射光代替
uniform vec3 objectColor;
uniform vec3 viewPos;

//材质属性 定义当前材质在环境光照、漫反射、镜面光照情况下的颜色  与对应光的颜色相乘 就是我们看到该材质在阳光下的颜色
struct Material {
	//diffuse之前是vec3格式 代表一个颜色
	//现在改为sampler2D 将传入一个纹理
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
//对于Struct类型的uniform类型数据赋值 时name = {变量名}.{属性名} eg: setVec3("material.ambient",vec3(1.0f,1.0f,1.0f));
uniform Material material;

struct Light {
	vec3 position;
//	vec3 lightDir;

	//聚光效果 方向 内切角与外切角  内切角光强 外切角光逐渐减弱
	vec3 direction;
	float cutOff;
	float outerCutOff;


	//光源所产生的环境光 漫反射光  镜面反射光
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//在处理光线衰减时需要的三个参数 光线衰减有个方程
	float constant;
	float linear;
	float quadratic;
};
uniform Light light;
uniform float matrixmove;
uniform float matrixlight;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//#define NR_POINT_LIGHTS 4

uniform PointLight pointLights[4];

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light,vec3 normal, vec3 viewDir);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
	/*vec3 ambient = light.ambient * material.ambient;  单纯的添加漫反射与镜面反射
	//normalize方法是为了将向量标准化 化为单位向量  因为在这里面，我们不关心向量的长度与位置
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	//将法向量与光源方向点乘 计算光源对漫反射的影响 a * b = |a| * |b| * cosθ  因为ab都是单位向量所以|a|与|b|都是1
	float diff = max(dot(norm, lightDir), 0.0);
	//然后乘以光的颜色  最终得到漫反射后的颜色
	vec3 diffuse = lightColor * (diff * material.diffuse);

	//镜反射光照s
	vec3 viewDir = normalize(viewPos - FragPos); //计算视觉向量

	vec3 reflectDir = reflect(-lightDir, norm); //计算光反射向量  -lightDir 是光源指向片段的向量  norm是法向量  最后得到反射向量
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);*/

	
	
	//各种反射 + 材质 + 聚光与光衰减等效果
	//计算片段到光源的方向
	/*vec3 lightDir = normalize(light.position - FragPos);

	float theta = dot(lightDir,normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon , 0.0 , 1.0);

	// 环境光
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	// 漫反射 
	vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(-light.lightDir);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, TexCoords).rgb);

	// 镜面光
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);

	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + distance * light.linear + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	diffuse *= intensity;
	specular *= intensity;
	//vec3 emission = texture(material.emission, vec2(TexCoords.x, TexCoords.y + matrixmove)).rgb;
	//emission *= attenuation;
	//+emission
	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);  */
	
/*	if (theta > light.cutOff) {

	}
	else {
		FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
	}*/

	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	for (int i = 0; i < 4; i++) {
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0);

}

//normal 法向量  viewDir是摄像

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	vec3 reflectDir = reflect(-lightDir,normal);
	float spec = pow(max(dot(reflectDir, normal), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light,vec3 normal, vec3 fragPos,vec3 viewDir) {

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords));

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}


