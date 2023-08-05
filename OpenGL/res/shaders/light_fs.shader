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

void main() {
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	//normalize������Ϊ�˽�������׼�� ��Ϊ��λ����  ��Ϊ�������棬���ǲ����������ĳ�����λ��
	vec3 norm = normalize(Normal); 
	vec3 lightDir = normalize(lightPos - FragPos);
	//�����������Դ������ �����Դ���������Ӱ�� a * b = |a| * |b| * cos��  ��Ϊab���ǵ�λ��������|a|��|b|����1
	float diff = max(dot(norm, lightDir), 0.0);
	//Ȼ����Թ����ɫ  ���յõ�����������ɫ
	vec3 diffuse = diff * lightColor;

	//���������s
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos); //�����Ӿ�����

	vec3 reflectDir = reflect(-lightDir,norm); //����ⷴ������  -lightDir �ǹ�Դָ��Ƭ�ε�����  norm�Ƿ�����  ���õ���������
	float spec = pow(max(dot(viewDir,reflectDir),0.0),256);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	//FragColor = vec4(lightColor * objectColor, 1.0f);
	FragColor = vec4(result, 1.0f);

}
