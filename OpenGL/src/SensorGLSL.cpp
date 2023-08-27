#include "cubeMap.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

#include "Shader.h"

//需要提前定义此标识符，确保stb_image的使用
//#define STB_IMAGE_IMPLEMENTATION
#include "GLTextureImage.h"

#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#pragma pack(2) //影响对齐，参考结构体内存空间分配规则

struct SeniorCameraLight {
	glm::vec3 cameraLightPos = glm::vec3(0.0f, 0.0f, 15.0f);
	glm::vec3 cameraLightFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraLightUp = glm::vec3(0.0f, 1.0f, 0.0f);
};


struct SeniorlightSeniorConstant {
	float deltaLightTime = 0.0f;
	float lastLightFrame = 0.0f;
	float lastLightX = 400, lastLightY = 300;
	float yawLight = -90.0f, pitchLight = 0.0f;
	float fovLight = 45.0;
	bool firstLightMouse = true;
};

struct SeniorwindowSeniorConstant
{
	int screenWidth = 800;
	int screenHeight = 800;
};

SeniorCameraLight cameraSeniorConstant;
SeniorlightSeniorConstant lightSeniorConstant;
SeniorwindowSeniorConstant windowSeniorConstant;

void mouse_senior_light_callback(GLFWwindow* window, double xpos, double ypos);
void process_senior_light_Input(GLFWwindow *window);

int main_senior() {
	GLFWwindow *window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(windowSeniorConstant.screenWidth, windowSeniorConstant.screenHeight, "SeniorTest", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_senior_light_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetFramebufferSize(window, &windowSeniorConstant.screenWidth, &windowSeniorConstant.screenHeight);

	glViewport(0, 0, windowSeniorConstant.screenWidth, windowSeniorConstant.screenHeight);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error init!" << std::endl;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);

	float points[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};


	float normals[] = {
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,

		 0.0f,  0.0f, 1.0f, 
		 0.0f,  0.0f, 1.0f, 
		 0.0f,  0.0f, 1.0f, 
		 0.0f,  0.0f, 1.0f, 
		 0.0f,  0.0f, 1.0f, 
		 0.0f,  0.0f, 1.0f, 

		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,

		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,

		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f
	};

	float texCoords[] = {
		  0.0f, 0.0f,
		  1.0f, 0.0f,
		  1.0f, 1.0f,
		  1.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 0.0f,

		  0.0f, 0.0f,
		  1.0f, 0.0f,
		  1.0f, 1.0f,
		  1.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 0.0f,

		  1.0f, 0.0f,
		  1.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 0.0f,
		  1.0f, 0.0f,

		  1.0f, 0.0f,
		  1.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 1.0f,
		  0.0f, 0.0f,
		  1.0f, 0.0f,

		  0.0f, 1.0f,
		  1.0f, 1.0f,
		  1.0f, 0.0f,
		  1.0f, 0.0f,
		  0.0f, 0.0f,
		  0.0f, 1.0f,

		  0.0f, 1.0f,
		  1.0f, 1.0f,
		  1.0f, 0.0f,
		  1.0f, 0.0f,
		  0.0f, 0.0f,
		  0.0f, 1.0f
	};

	unsigned int pVAO, pVBO;
	glGenVertexArrays(1,&pVAO);
	glGenBuffers(1, &pVBO);

	glBindVertexArray(pVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(texCoords), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), &points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(texCoords), &texCoords);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)sizeof(points));
	glEnableVertexAttribArray(1);

	Shader pointShader = Shader("res/shaders/point/point_vs.shader", "res/shaders/point/point_fs.shader");
	Shader cubeShader = Shader("res/shaders/point/fragCoord_vs.shader", "res/shaders/point/fragCoord_fs.shader");

	pointShader.use();
	cubeShader.use();

	GLTextureImage cubeImageTexture = GLTextureImage("res/image/container2.png");
	GLTextureImage cubeInImageTexture = GLTextureImage("res/image/awesomeface.png");

	cubeShader.setInt("boxTextureOut", 0);
	cubeShader.setInt("boxTextureIn", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		lightSeniorConstant.deltaLightTime = currentFrame - lightSeniorConstant.lastLightFrame;
		lightSeniorConstant.lastLightFrame = currentFrame;

		process_senior_light_Input(window);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraSeniorConstant.cameraLightPos, cameraSeniorConstant.cameraLightPos + cameraSeniorConstant.cameraLightFront, cameraSeniorConstant.cameraLightUp);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pointShader.use();
		pointShader.setMat4("model", model);
		pointShader.setMat4("view", view);
		pointShader.setMat4("projection", projection);
		glBindVertexArray(pVAO);
		glDrawArrays(GL_POINTS, 0, 36);

		cubeShader.use();
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE, cubeImageTexture.textureId);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE1, cubeInImageTexture.textureId);
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &pVAO);
	glDeleteBuffers(1, &pVBO);

	glfwTerminate();
	return 1;
}

void mouse_senior_light_callback(GLFWwindow *window, double xposIn, double yposIn) {

	//	std::cout << "鼠标输入" << std::endl;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (lightSeniorConstant.firstLightMouse)
	{
		lightSeniorConstant.lastLightX = xpos;
		lightSeniorConstant.lastLightY = ypos;
		lightSeniorConstant.firstLightMouse = false;
	}
	float xoffset = xpos - lightSeniorConstant.lastLightX;
	float yoffset = lightSeniorConstant.lastLightY - ypos;

	lightSeniorConstant.lastLightX = xpos;
	lightSeniorConstant.lastLightY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	lightSeniorConstant.yawLight += xoffset;
	lightSeniorConstant.pitchLight += yoffset;

	if (lightSeniorConstant.pitchLight > 89.0f)
	{
		lightSeniorConstant.pitchLight = 89.0f;
	}
	if (lightSeniorConstant.pitchLight < -89.0f)
	{
		lightSeniorConstant.pitchLight = -89.0f;
	}
	/**/
	glm::vec3 front;
	front.x = cos(glm::radians(lightSeniorConstant.yawLight)) * cos(glm::radians(lightSeniorConstant.pitchLight));
	front.y = sin(glm::radians(lightSeniorConstant.pitchLight));
	front.z = sin(glm::radians(lightSeniorConstant.yawLight)) * cos(glm::radians(lightSeniorConstant.pitchLight));
	cameraSeniorConstant.cameraLightFront = glm::normalize(front);


}

void process_senior_light_Input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * lightSeniorConstant.deltaLightTime;

	//std::cout << "修改前 ： x = " << cameraSeniorConstant.cameraLightPos.x << " , y = " << cameraSeniorConstant.cameraLightPos.y << " , z = " << cameraSeniorConstant.cameraLightPos.z << std::endl;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		std::cout << "按键W" << std::endl;
		cameraSeniorConstant.cameraLightPos += cameraSpeed * cameraSeniorConstant.cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		std::cout << "按键S" << std::endl;
		cameraSeniorConstant.cameraLightPos -= cameraSpeed * cameraSeniorConstant.cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		std::cout << "按键A" << std::endl;
		cameraSeniorConstant.cameraLightPos -= glm::normalize(glm::cross(cameraSeniorConstant.cameraLightFront, cameraSeniorConstant.cameraLightUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		std::cout << "按键D" << std::endl;
		cameraSeniorConstant.cameraLightPos += glm::normalize(glm::cross(cameraSeniorConstant.cameraLightFront, cameraSeniorConstant.cameraLightUp)) * cameraSpeed;
	}

	//std::cout << "修改后 ： x = " << cameraSeniorConstant.cameraLightPos.x << " , y = " << cameraSeniorConstant.cameraLightPos.y << " , z = " << cameraSeniorConstant.cameraLightPos.z << std::endl;


}