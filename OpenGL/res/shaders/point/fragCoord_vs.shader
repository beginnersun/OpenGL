#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;// 移除纹理位置，采用反射sky纹理取颜色

// std140声明的方式能够在Matrices的预留内存中设置好每个变量的内存位置与对应的偏移量。
//如果不指定，使用默认的Shared（共享布局），GLSL会自己优化对应的内存布局方式，我们就需要使用其他方式
//例如：glGetUniformIndices查询偏移量。
//使用std140需要手动计算 偏移量

//以下有两个知识点：
//1.基准对其量  （在空间中占用的具体大小） int bool float 都是4字节
//2.对齐偏移量   注意！！对齐偏移量是其基准对其量的倍数
//eg:           基准            偏移
// float value     4              0
// mat4  cc       16             (4?错滴)是16  倍数注意噻
// int   dd        4             16 没毛病16是4的倍数
// 基准的基本规则 除了int bool float 是4字节外(四个字节用N表示）
// vec2 = 2N  , vec4 = 4N 然后vec3 == 4N 哈哈哈哈
// 向量  向量数组的元素 都是以vec4相同  结构体 矩阵则会填充到vec4的倍数
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