#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
//��Ҫ��ǰ����˱�ʶ����ȷ��stb_image��ʹ��
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class BaseGL
{
public:
	BaseGL();
	~BaseGL();

	void mouse_senior_light_callback(GLFWwindow* window, double xpos, double ypos);
	void process_senior_light_Input(GLFWwindow *window);

private:

};

BaseGL::BaseGL()
{
}

BaseGL::~BaseGL()
{
}