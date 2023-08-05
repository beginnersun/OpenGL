#version 330 core
out vec4 FragColor;

//��Ӧ��ƽ��ķ�����
in vec3 Normal;
//Ƭ����ɫ���еĵ�ǰ����λ�ã�����ռ��еģ�
in vec3 FragPos;

in vec2 TexCoords;

//���ݹ�Դ��λ�� �뵱ǰ��Ƭ������  ������������յ���ɫ
//uniform vec3 lightPos;   �滻Ϊ�ṹ���е�Light->position ����
//uniform vec3 lightColor; lightColor ����ʹ�ã����Ǳ�����������
uniform vec3 objectColor;
uniform vec3 viewPos;

//�������� ���嵱ǰ�����ڻ������ա������䡢�����������µ���ɫ  ���Ӧ�����ɫ��� �������ǿ����ò����������µ���ɫ
struct Material {
	//diffuse֮ǰ��vec3��ʽ ����һ����ɫ
	//���ڸ�Ϊsampler2D ������һ������
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};
//����Struct���͵�uniform�������ݸ�ֵ ʱname = {������}.{������} eg: setVec3("material.ambient",vec3(1.0f,1.0f,1.0f));
uniform Material material;

struct Light {
	vec3 position;
//	vec3 lightDir;

	//�۹�Ч�� ���� ���н������н�  ���нǹ�ǿ ���нǹ��𽥼���
	vec3 direction;
	float cutOff;
	float outerCutOff;


	//��Դ�������Ļ����� �������  ���淴���
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//�ڴ������˥��ʱ��Ҫ���������� ����˥���и�����
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
	/*vec3 ambient = light.ambient * material.ambient;  ����������������뾵�淴��
	//normalize������Ϊ�˽�������׼�� ��Ϊ��λ����  ��Ϊ�������棬���ǲ����������ĳ�����λ��
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	//�����������Դ������ �����Դ���������Ӱ�� a * b = |a| * |b| * cos��  ��Ϊab���ǵ�λ��������|a|��|b|����1
	float diff = max(dot(norm, lightDir), 0.0);
	//Ȼ����Թ����ɫ  ���յõ�����������ɫ
	vec3 diffuse = lightColor * (diff * material.diffuse);

	//���������s
	vec3 viewDir = normalize(viewPos - FragPos); //�����Ӿ�����

	vec3 reflectDir = reflect(-lightDir, norm); //����ⷴ������  -lightDir �ǹ�Դָ��Ƭ�ε�����  norm�Ƿ�����  ���õ���������
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);*/

	
	
	//���ַ��� + ���� + �۹����˥����Ч��
	//����Ƭ�ε���Դ�ķ���
	/*vec3 lightDir = normalize(light.position - FragPos);

	float theta = dot(lightDir,normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon , 0.0 , 1.0);

	// ������
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	// ������ 
	vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(-light.lightDir);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * texture(material.diffuse, TexCoords).rgb);

	// �����
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

//normal ������  viewDir������

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


