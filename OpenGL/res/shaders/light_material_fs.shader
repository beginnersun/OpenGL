#version 330 core
out vec4 FragColor;

//��Ӧ��ƽ��ķ�����
in vec3 Normal;
//Ƭ����ɫ���еĵ�ǰ����λ�ã�����ռ��еģ�
in vec3 FragPos;

//���ݹ�Դ��λ�� �뵱ǰ��Ƭ������  ������������յ���ɫ
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos; 

//�������� ���嵱ǰ�����ڻ������ա������䡢�����������µ���ɫ  ���Ӧ�����ɫ��� �������ǿ����ò����������µ���ɫ
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
//����Struct���͵�uniform�������ݸ�ֵ ʱname = {������}.{������} eg: setVec3("material.ambient",vec3(1.0f,1.0f,1.0f));
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
	//normalize������Ϊ�˽�������׼�� ��Ϊ��λ����  ��Ϊ�������棬���ǲ����������ĳ�����λ��
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	//�����������Դ������ �����Դ���������Ӱ�� a * b = |a| * |b| * cos��  ��Ϊab���ǵ�λ��������|a|��|b|����1
	float diff = max(dot(norm, lightDir), 0.0);
	//Ȼ����Թ����ɫ  ���յõ�����������ɫ
	vec3 diffuse = lightColor * (diff * material.diffuse);

	//���������s
	vec3 viewDir = normalize(viewPos - FragPos); //�����Ӿ�����

	vec3 reflectDir = reflect(-lightDir, norm); //����ⷴ������  -lightDir �ǹ�Դָ��Ƭ�ε�����  norm�Ƿ�����  ���õ���������
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0f);*/

	// ������
	vec3 ambient = light.ambient * material.ambient;

	// ������ 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// �����
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);

}
