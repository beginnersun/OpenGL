#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;// �Ƴ�����λ�ã����÷���sky����ȡ��ɫ

// std140�����ķ�ʽ�ܹ���Matrices��Ԥ���ڴ������ú�ÿ���������ڴ�λ�����Ӧ��ƫ������
//�����ָ����ʹ��Ĭ�ϵ�Shared�������֣���GLSL���Լ��Ż���Ӧ���ڴ沼�ַ�ʽ�����Ǿ���Ҫʹ��������ʽ
//���磺glGetUniformIndices��ѯƫ������
//ʹ��std140��Ҫ�ֶ����� ƫ����

//����������֪ʶ�㣺
//1.��׼������  ���ڿռ���ռ�õľ����С�� int bool float ����4�ֽ�
//2.����ƫ����   ע�⣡������ƫ���������׼�������ı���
//eg:           ��׼            ƫ��
// float value     4              0
// mat4  cc       16             (4?���)��16  ����ע����
// int   dd        4             16 ûë��16��4�ı���
// ��׼�Ļ������� ����int bool float ��4�ֽ���(�ĸ��ֽ���N��ʾ��
// vec2 = 2N  , vec4 = 4N Ȼ��vec3 == 4N ��������
// ����  ���������Ԫ�� ������vec4��ͬ  �ṹ�� ���������䵽vec4�ı���
layout(std140, binding = 0) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

out VS_OUT
{
	vec2 TexCoords;
}vs_out;


uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;

void main(){
	vs_out.TexCoords = aTexCoords;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}