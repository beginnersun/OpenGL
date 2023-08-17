#include <stdio.h>
#include "FrameBufferTest.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"

//��Ҫ��ǰ����˱�ʶ����ȷ��stb_image��ʹ��
//#define STB_IMAGE_IMPLEMENTATION
#include "GLTextureImage.h"

#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#pragma pack(2) //Ӱ����룬�ο��ṹ���ڴ�ռ������� 


//fread���� ��ȡint����������ʱ����С�����ȡ  ����int�������ݰ��մ�����ȡ
//�����ڶ�ȡ��ֵ֮����Ҫ�Դ�С�������ת�� 
unsigned long OnChangeByteOrder(int indata)
{
	unsigned char *c = (unsigned char*)&indata;
	int sum = 0;
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += (*(c + i) << ((sizeof(int) - 1 - i) * 8));
	}
	//printf("ת���Ľ�� : %d \n",sum);
	return sum;
}


glm::vec3 cameraLightPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraLightFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraLightUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//glm::vec3 lightPos(1.2f, 0.0f, 2.0f);
void mouse_light_callback(GLFWwindow* window, double xpos, double ypos);
void process_light_Input(GLFWwindow *window);
void saveToImage(unsigned char* data, int width, int height);
//unsigned int loadImage(const char *path);

int saveCount = 0;

float deltaLightTime = 0.0f;
float lastLightFrame = 0.0f;
float lastLightX = 400, lastLightY = 300;
float yawLight = -90.0f, pitchLight = 0.0f;
float fovLight = 45.0;
bool firstLightMouse = true;

typedef unsigned char BYTE;
typedef unsigned short Word;
typedef unsigned long DWord;
typedef long Long;

//ͼƬɨ�跽ʽ �����Ҵ��µ���ɨ��������Ϣ

/*�ļ���Ϣͷ*/
struct BitmapFileHeader {
	Word bfType; //���ֽڹ̶�����BM
	DWord bfSize; //����BMP�ļ��Ĵ�С
	Word bfReserved1; // 0
	Word bfReserved2; // 0
	DWord bfOffBits; //�ļ���ʼλ�õ��������ݵ�ƫ����
};

/*λͼ��Ϣͷ*/
struct BitmapInfoHeader {
	DWord biSize; //�˽ṹ��ռ�õĴ�С �̶�40
	Long biWidth; //ͼƬ��
	Long biHeight; //ͼƬ��
	Word biPanels; //ƽ���� 1
	Word biBitCount; //����λ�� 1 2 8 16 24 32
	DWord biCompression; // ѹ����ʽ 0 ��ѹ�� 1 RLE8 2 RLE4
	DWord biSizeImage; // 4�ֽڶ����ͼ�����ݴ�С
	Long biXPelsPermeter; //X����ķֱ���
	Long biYPelsPermeter; //Y....�ķֱ���
	DWord biClrUsed;      //ʵ�ʵ�ɫ�������� 0 �������е�ɫ������
	DWord biClrImportant; //��Ҫ��ɫ�������� 0 ���ж���Ҫ
};

int main_framebuffer() {

	GLFWwindow *window;

	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(800, 600, "Hello Light", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}




	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_light_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	std::cout << "width = " << width << "height = " << height << std::endl;

	glViewport(0, 0, 800, 600);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error !" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	//glEnable(GL_DEPTH_TEST);

	float positions[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	unsigned int VBO, cubeVAO;
	//cubeVAO ��Ӧ��������������Ķ�������
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//��Դ�Ķ�������
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//�������������ͼ�����꣨2d���Ӵ��
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	   // positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};
	
	unsigned int quadVAO;
	unsigned int quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//stbi_set_flip_vertically_on_load(true);

	//���������Ӧ��ID
	unsigned int texture;
	glGenTextures(1, &texture);
	int nrChannels;
	unsigned char *data = stbi_load("res/image/container2.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture);

		//std::cout << "�������� : " << data << std::endl;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << "" << std::endl;
		stbi_image_free(data);
	}

	GLTextureImage specularTexture = GLTextureImage("res/image/lighting_maps_specular_color.png");
	GLTextureImage emissionTexture = GLTextureImage("res/image/matrix.jpg");

	std::cout << "emissionTexture.id = " << emissionTexture.textureId << std::endl;

	Shader lightingShader = Shader("res/shaders/light_diffuse_map_vs.shader", "res/shaders/light_diffuse_map_fs.shader"); //��Ӧ���������������ɫ��
	Shader cubeShader = Shader("res/shaders/cube_vs.shader", "res/shaders/cube_fs.shader");  //�����Ӧ����ʵ�ǹ�Դ����ɫ��
	Shader screenShader = Shader("res/shaders/texture/texture_vs.shader", "res/shaders/texture/texture_fs.shader");

	width = 800;
	height = 600;

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//�������� ��ɫ��|��ɫ����Ϣ����Ƶ���������   
	unsigned int colourTextureAppend;
	glGenTextures(1, &colourTextureAppend);
	glBindTexture(GL_TEXTURE_2D, colourTextureAppend);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//GL_COLOR_ATTACHMENT0 ����Ҫ�󶨵���ɫ�ʸ��� Ҳ���԰嶥��Ȼ���ģ����Ϣ����ɶ�ģ�����һ�㲻�����ַ�ʽ��
	//glBindTexture(GL_TEXTURE_2D, 0); //���Texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTextureAppend, 0);

	//����Ⱦ������� renderBufferObject
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//�˴��Ϳ���������Ⱦ������󴴽�һ�������ģ�建�帽��   ��Ⱦ�������ֻд����������ʹ��glReadPixels������ȡ֡���壬����ֻ֧��д�룬���ʺ������ģ����Ϣ��
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer is Error;!!!!!!!!!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,0.2f,2.0f),
		glm::vec3(2.3f,-3.3f,-4.0f),
		glm::vec3(-4.0f,2.0f,-12.0f),
		glm::vec3(0.0f,0.0f,-3.0f)
	};

	unsigned char *imageData = NULL;


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaLightTime = currentFrame - lastLightFrame;
		lastLightFrame = currentFrame;
		process_light_Input(window);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.use();

		//lightingShader.setVec3("lightColor",glm::vec3(1.0f, 0.5f, 0.31f));
		//lightingShader.setVec3("objectColor",glm::vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
		lightingShader.setVec3("light.direction", cameraLightFront);
		lightingShader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
		lightingShader.setVec3("viewPos", cameraLightPos);

		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[0].constant", 1.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09f);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09f);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09f);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09f);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032f);

		// spotLight
		lightingShader.setVec3("spotLight.position", cameraLightFront);
		lightingShader.setVec3("spotLight.direction", cameraLightUp);
		lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09f);
		lightingShader.setFloat("spotLight.quadratic", 0.032f);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		//	lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
		//	lightPos.z = sin(glfwGetTime() / 2.0f) * 1.0f;

		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
		lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
		lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));

		glm::vec3 ambientColor = glm::vec3(0.2f);
		glm::vec3 diffuseColor = glm::vec3(0.5);
		glm::vec3 specularColor = glm::vec3(1.0f);

		lightingShader.setVec3("light.ambient", ambientColor);
		lightingShader.setVec3("light.diffuse", diffuseColor);
		lightingShader.setVec3("light.specular", specularColor);

		//std::cout << "�ƶ����� " << (1.0 + sin(glfwGetTime()) / 2.0 + 0.5) << std::endl;
		lightingShader.setFloat("matrixmove", glfwGetTime());
		lightingShader.setFloat("matrixlight", (1.0 + sin(glfwGetTime())) / 2 + 0.5);

		lightingShader.setInt("material.diffuse", 0);
		lightingShader.setInt("material.specular", 1);
		lightingShader.setInt("material.emission", 2);

		/*lightingShader.setVec3("material.ambient",glm::vec3(1.0f,1.0f,1.0f));
		lightingShader.setVec3("material.diffuse",glm::vec3(1.0f, 1.0f, 1.0f));;*/
		//lightingShader.setVec3("material.specular",glm::vec3(1.0f, 1.0f, 1.0f));

		lightingShader.setFloat("material.shininess", 32.0f);

		lightingShader.setFloat("light.constant", 1.0f);
		lightingShader.setFloat("light.linear", 0.09f);
		lightingShader.setFloat("light.quadratic", 0.032f);

		for (int i = 0; i < 4; i++)
		{

			//lightingShader.setFloat("pointLight");
		}


		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::lookAt(cameraLightPos, cameraLightPos + cameraLightFront, cameraLightUp);

		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTexture.textureId);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, emissionTexture.textureId);

		glBindVertexArray(cubeVAO);

		for (int i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)(1.0f) *glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
			lightingShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}


		cubeShader.use();


		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		cubeShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colourTextureAppend);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
		glfwSwapBuffers(window);

		glfwPollEvents();

		if (saveCount > 0) {
			std::cout << "׼�����ͼƬ" << std::endl;
			imageData = new unsigned char[4 * width * height];
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
			// OPEN_GL��ǰ��ɫ����������Ⱦ | GL_BACK �ں���ɫ����������Ⱦ
			//glReadBuffer(GL_FRONT);
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, imageData);
			saveToImage(imageData,width,height);
			saveCount--;
		}
	}



	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &quadVAO);
	//glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);

	glfwTerminate();
	return 1;
}

/* BMPͼƬ���� ֧��RGB���ݣ�ľ��A�� */
void saveToImage(unsigned char* data,int width,int height) {
	const int colorBufferSize = width * height * sizeof(char) * 3;

	std::cout << "wdith = " << width << " , height = " << height << "" << std::endl;
	
	BitmapFileHeader fileHeader;
	fileHeader.bfType = 0X4D42; // BM��16����
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
	fileHeader.bfSize = fileHeader.bfOffBits + colorBufferSize;

	BitmapInfoHeader infoHeader;
	infoHeader.biSize = sizeof(BitmapInfoHeader);
	infoHeader.biWidth = width;
	infoHeader.biHeight = height;
	std::cout << "wdith = " << infoHeader.biWidth << " , height = " << infoHeader.biHeight << "" << std::endl;
	infoHeader.biPanels = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = colorBufferSize;


	std::string fileName = "res/image/outImage.bmp";
	FILE *imageFile;
	fopen_s(&imageFile, fileName.c_str(), "wb");
	fwrite(&fileHeader, sizeof(BitmapFileHeader), 1, imageFile);
	fwrite(&infoHeader, sizeof(BitmapInfoHeader), 1, imageFile);
	fwrite(data, colorBufferSize, 1, imageFile);

	fclose(imageFile);

	/*
	FILE *imageFile = fopen(fileName.c_str(), "w");
	fprintf(imageFile, ); */
}


void mouse_light_callback(GLFWwindow *window, double xposIn, double yposIn) {

	//	std::cout << "�������" << std::endl;

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstLightMouse)
	{
		lastLightX = xpos;
		lastLightY = ypos;
		firstLightMouse = false;
	}
	float xoffset = xpos - lastLightX;
	float yoffset = lastLightY - ypos;

	lastLightX = xpos;
	lastLightY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yawLight += xoffset;
	pitchLight += yoffset;

	if (pitchLight > 89.0f)
	{
		pitchLight = 89.0f;
	}
	if (pitchLight < -89.0f)
	{
		pitchLight = -89.0f;
	}
	/**/
	glm::vec3 front;
	front.x = cos(glm::radians(yawLight)) * cos(glm::radians(pitchLight));
	front.y = sin(glm::radians(pitchLight));
	front.z = sin(glm::radians(yawLight)) * cos(glm::radians(pitchLight));
	cameraLightFront = glm::normalize(front);


}

void process_light_Input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaLightTime;


	std::cout << "�޸�ǰ �� x = " << cameraLightPos.x << " , y = " << cameraLightPos.y << " , z = " << cameraLightPos.z << std::endl;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		std::cout << "����W" << std::endl;
		cameraLightPos += cameraSpeed * cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		std::cout << "����S" << std::endl;
		saveCount++;
		cameraLightPos -= cameraSpeed * cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		std::cout << "����A" << std::endl;
		cameraLightPos -= glm::normalize(glm::cross(cameraLightFront, cameraLightUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		std::cout << "����D" << std::endl;
		cameraLightPos += glm::normalize(glm::cross(cameraLightFront, cameraLightUp)) * cameraSpeed;
	}

	std::cout << "�޸ĺ� �� x = " << cameraLightPos.x << " , y = " << cameraLightPos.y << " , z = " << cameraLightPos.z << std::endl;

}