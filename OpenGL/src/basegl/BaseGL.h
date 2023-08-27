#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
//需要提前定义此标识符，确保stb_image的使用
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