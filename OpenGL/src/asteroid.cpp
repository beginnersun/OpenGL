#include "asteroid.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "Shader.h"
#include "Model.h"

//需要提前定义此标识符，确保stb_image的使用
//#define STB_IMAGE_IMPLEMENTATION
#include "GLTextureImage.h"

#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#pragma pack(2) //影响对齐，参考结构体内存空间分配规则

struct AsteroidCameraLight {
	glm::vec3 cameraLightPos = glm::vec3(0.0f, 0.0f, 15.0f);
	glm::vec3 cameraLightFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraLightUp = glm::vec3(0.0f, 1.0f, 0.0f);
};


struct AsteroidlightAsteroidConstant {
	float deltaLightTime = 0.0f;
	float lastLightFrame = 0.0f;
	float lastLightX = 400, lastLightY = 300;
	float yawLight = -90.0f, pitchLight = 0.0f;
	float fovLight = 45.0;
	bool firstLightMouse = true;
};

struct AsteroidwindowAsteroidConstant
{
	int screenWidth = 800;
	int screenHeight = 800;
};

AsteroidCameraLight cameraSeniorConstant;
AsteroidlightAsteroidConstant lightSeniorConstant;
AsteroidwindowAsteroidConstant windowSeniorConstant;

void mouse_asteroid_light_callback(GLFWwindow* window, double xpos, double ypos);
void process_asteroid_light_Input(GLFWwindow *window);

int main_asteroid() {


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
	glfwSetCursorPosCallback(window, mouse_asteroid_light_callback);
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


	Shader rockShader = Shader("res/shaders/asteroid/model_rock_vs.shader", "res/shaders/asteroid/model_rock_fs.shader");

	Shader linePointShader = Shader("res/shaders/asteroid/model_loading_vs.shader", "res/shaders/asteroid/model_loading_fs.shader");
	stbi_set_flip_vertically_on_load(true);
	Model asteroidModel = Model("res/model/rock/planet.obj");
	Model rockModel = Model("res/model/rock/rock.obj");

	unsigned int amount = 10000;
	glm::mat4 *modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(static_cast<unsigned int>(glfwGetTime()));
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);

		float angle = (float)i / (float)amount*360.0f;
		float displacement = (rand() % (int)(2 * offset)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = static_cast<float>((rand() % 360));
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rockModel.meshes.size(); i++)
	{
		unsigned int VAO = rockModel.meshes[i].VAO;
		glBindVertexArray(VAO);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);

		glBindVertexArray(0);
	}

	unsigned int uboExampleBlock;
	glGenBuffers(1, &uboExampleBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowSeniorConstant.screenWidth / (float)windowSeniorConstant.screenHeight, 1.0f, 1000.0f);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboExampleBlock, 0, sizeof(glm::mat4));
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = static_cast<float>(glfwGetTime());
		lightSeniorConstant.deltaLightTime = currentFrame - lightSeniorConstant.lastLightFrame;
		lightSeniorConstant.lastLightFrame = currentFrame;

		process_asteroid_light_Input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraSeniorConstant.cameraLightPos, cameraSeniorConstant.cameraLightPos + cameraSeniorConstant.cameraLightFront, cameraSeniorConstant.cameraLightUp);
		glm::mat4 model = glm::mat4(1.0f);
		rockShader.use();
		rockShader.setMat4("view", view);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		rockShader.setMat4("model", model);

		asteroidModel.Draw(rockShader);

		linePointShader.use();
		linePointShader.setMat4("view", view);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockModel.textures_loaded[0].id);
		for (unsigned int i = 0; i < rockModel.meshes.size(); i++)
		{
			glBindVertexArray(rockModel.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(rockModel.meshes[i].indices.size()), GL_UNSIGNED_INT, 0, amount);
			glBindVertexArray(0);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 1;

}


void mouse_asteroid_light_callback(GLFWwindow *window, double xposIn, double yposIn) {

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


void process_asteroid_light_Input(GLFWwindow *window) {

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
