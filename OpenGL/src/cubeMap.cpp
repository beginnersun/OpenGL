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

struct CubeCameraLight {
	glm::vec3 cameraLightPos = glm::vec3(0.0f, 0.0f, 15.0f);
	glm::vec3 cameraLightFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraLightUp = glm::vec3(0.0f, 1.0f, 0.0f);
};

void mouse_cube_light_callback(GLFWwindow* window, double xpos, double ypos);
void process_cube_light_Input(GLFWwindow *window);
void saveToImage(unsigned char* data, int width, int height);

int loadSkyMaps(std::vector<std::string> images);

struct LightConstant {
	float deltaLightTime = 0.0f;
	float lastLightFrame = 0.0f;
	float lastLightX = 400, lastLightY = 300;
	float yawLight = -90.0f, pitchLight = 0.0f;
	float fovLight = 45.0;
	bool firstLightMouse = true;
};

struct WindowConstant
{
	int screenWidth = 800;
	int screenHeight = 800;
};

CubeCameraLight cameraConstant;
LightConstant lightConstant;
WindowConstant windowConstant;

int main_cubemap() {
	GLFWwindow *window;

	if (!glfwInit()) {
		return -1;
	}

	window = glfwCreateWindow(windowConstant.screenWidth, windowConstant.screenHeight, "CubeMapTest", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_cube_light_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetFramebufferSize(window, &windowConstant.screenWidth, &windowConstant.screenHeight);

	glViewport(0, 0, windowConstant.screenWidth, windowConstant.screenHeight);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error init!" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
/*  箱子采用原本texture纹理
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	}; */
	//箱子采用 反射天空纹理
	float cubeVertices[] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	float skyboxVertices[] = {
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

	//delay vao
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int skyVAO, skyVBO;
	glGenVertexArrays(1, &skyVAO);
	glGenBuffers(1, &skyVBO);
	glBindVertexArray(skyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Shader skyShader = Shader("res/shaders/sky/sky_vs.shader","res/shaders/sky/sky_fs.shader");
	Shader cubeShader = Shader("res/shaders/sky/cube_vs.shader", "res/shaders/sky/cube_fs.shader");

	//GLTextureImage cubeImageTexture = GLTextureImage("res/image/container2.png");

	cubeShader.use();
	cubeShader.setInt("skybox", 0);
	//cubeShader.setInt("texture1", 0);

	skyShader.use();
	skyShader.setInt("skybox", 0);

	//std::cout << " cube textureId = " << cubeImageTexture.textureId << std::endl;

	std::vector<std::string> cubeImages{
		"res/image/skybox/right.jpg",
		"res/image/skybox/left.jpg",
		"res/image/skybox/top.jpg",
		"res/image/skybox/bottom.jpg",
		"res/image/skybox/front.jpg",
		"res/image/skybox/back.jpg"
	};
	int skyTexture = loadSkyMaps(cubeImages);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		lightConstant.deltaLightTime = currentFrame - lightConstant.lastLightFrame;
		lightConstant.lastLightFrame = currentFrame;

		process_cube_light_Input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f*windowConstant.screenWidth / windowConstant.screenHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(0.0f);
		view = glm::lookAt(cameraConstant.cameraLightPos, cameraConstant.cameraLightPos + cameraConstant.cameraLightFront, cameraConstant.cameraLightUp);
		glm::mat4 model = glm::mat4(1.0f);
		float angle = 20.0f * 1;
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

		cubeShader.setMat4("view", view);
		cubeShader.setMat4("model", model);
		cubeShader.setMat4("projection", projection);
		cubeShader.setVec3("cameraPos", cameraConstant.cameraLightPos);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, cubeImageTexture.textureId); 绑定箱子纹理
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		glDepthFunc(GL_LEQUAL);
		skyShader.use();
		skyShader.setMat4("projection", projection);
		skyShader.setMat4("view", glm::mat4(glm::mat3(view)));

		glBindVertexArray(skyVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteVertexArrays(1, &skyVAO);
	glDeleteBuffers(1, &skyVBO);

	glfwTerminate();
	return 1;
}

int loadSkyMaps(std::vector<std::string> images) {
	unsigned int skyTextureId;
	glGenTextures(1, &skyTextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);

	int imageWidth, imageHeight, nrChannels;
	unsigned char *data;

	for (unsigned int i = 0; i < images.size(); i++)
	{
		data = stbi_load(images[i].c_str(), &imageWidth, &imageHeight, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << "image load is Error , i = " << i << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return skyTextureId;
}


void mouse_cube_light_callback(GLFWwindow *window, double xposIn, double yposIn) {

	//	std::cout << "鼠标输入" << std::endl;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (lightConstant.firstLightMouse)
	{
		lightConstant.lastLightX = xpos;
		lightConstant.lastLightY = ypos;
		lightConstant.firstLightMouse = false;
	}
	float xoffset = xpos - lightConstant.lastLightX;
	float yoffset = lightConstant.lastLightY - ypos;

	lightConstant.lastLightX = xpos;
	lightConstant.lastLightY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	lightConstant.yawLight += xoffset;
	lightConstant.pitchLight += yoffset;

	if (lightConstant.pitchLight > 89.0f)
	{
		lightConstant.pitchLight = 89.0f;
	}
	if (lightConstant.pitchLight < -89.0f)
	{
		lightConstant.pitchLight = -89.0f;
	}
	/**/
	glm::vec3 front;
	front.x = cos(glm::radians(lightConstant.yawLight)) * cos(glm::radians(lightConstant.pitchLight));
	front.y = sin(glm::radians(lightConstant.pitchLight));
	front.z = sin(glm::radians(lightConstant.yawLight)) * cos(glm::radians(lightConstant.pitchLight));
	cameraConstant.cameraLightFront = glm::normalize(front);


}

void process_cube_light_Input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * lightConstant.deltaLightTime;

	//std::cout << "修改前 ： x = " << cameraConstant.cameraLightPos.x << " , y = " << cameraConstant.cameraLightPos.y << " , z = " << cameraConstant.cameraLightPos.z << std::endl;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		std::cout << "按键W" << std::endl;
		cameraConstant.cameraLightPos += cameraSpeed * cameraConstant.cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		std::cout << "按键S" << std::endl;
		cameraConstant.cameraLightPos -= cameraSpeed * cameraConstant.cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		std::cout << "按键A" << std::endl;
		cameraConstant.cameraLightPos -= glm::normalize(glm::cross(cameraConstant.cameraLightFront, cameraConstant.cameraLightUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		std::cout << "按键D" << std::endl;
		cameraConstant.cameraLightPos += glm::normalize(glm::cross(cameraConstant.cameraLightFront, cameraConstant.cameraLightUp)) * cameraSpeed;
	}

	//std::cout << "修改后 ： x = " << cameraConstant.cameraLightPos.x << " , y = " << cameraConstant.cameraLightPos.y << " , z = " << cameraConstant.cameraLightPos.z << std::endl;


}