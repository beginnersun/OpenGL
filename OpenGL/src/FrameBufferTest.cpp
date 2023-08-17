#include <stdio.h>
#include "FrameBufferTest.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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


//fread函数 读取int等类型数据时按照小端序读取  正常int解析数据按照大端序读取
//所以在读取到值之后需要对大小端序进行转换 
unsigned long OnChangeByteOrder(int indata)
{
	unsigned char *c = (unsigned char*)&indata;
	int sum = 0;
	for (int i = 0; i < sizeof(int); i++)
	{
		sum += (*(c + i) << ((sizeof(int) - 1 - i) * 8));
	}
	//printf("转序后的结果 : %d \n",sum);
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

//图片扫描方式 从左到右从下到上扫描像素信息

/*文件信息头*/
struct BitmapFileHeader {
	Word bfType; //两字节固定内容BM
	DWord bfSize; //整个BMP文件的大小
	Word bfReserved1; // 0
	Word bfReserved2; // 0
	DWord bfOffBits; //文件起始位置到像素数据的偏移量
};

/*位图信息头*/
struct BitmapInfoHeader {
	DWord biSize; //此结构体占用的大小 固定40
	Long biWidth; //图片宽
	Long biHeight; //图片高
	Word biPanels; //平面数 1
	Word biBitCount; //像素位数 1 2 8 16 24 32
	DWord biCompression; // 压缩方式 0 不压缩 1 RLE8 2 RLE4
	DWord biSizeImage; // 4字节对齐的图像数据大小
	Long biXPelsPermeter; //X方向的分辨率
	Long biYPelsPermeter; //Y....的分辨率
	DWord biClrUsed;      //实际调色板索引数 0 适用所有调色板索引
	DWord biClrImportant; //重要调色板索引数 0 所有都重要
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
	//cubeVAO 对应的是物体立方块的顶点数组
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

	//光源的顶点数组
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	//设置最后问题贴图的坐标（2d版的哟）
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

	//创建纹理对应的ID
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

		//std::cout << "加载数据 : " << data << std::endl;

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

	Shader lightingShader = Shader("res/shaders/light_diffuse_map_vs.shader", "res/shaders/light_diffuse_map_fs.shader"); //对应被照亮的物体的着色器
	Shader cubeShader = Shader("res/shaders/cube_vs.shader", "res/shaders/cube_fs.shader");  //这个对应的其实是光源的着色器
	Shader screenShader = Shader("res/shaders/texture/texture_vs.shader", "res/shaders/texture/texture_fs.shader");

	width = 800;
	height = 600;

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//绑定纹理附件 （色彩|颜色）信息会绘制到此纹理上   
	unsigned int colourTextureAppend;
	glGenTextures(1, &colourTextureAppend);
	glBindTexture(GL_TEXTURE_2D, colourTextureAppend);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//GL_COLOR_ATTACHMENT0 代表要绑定的是色彩附件 也可以板顶深度或者模板信息附件啥的（不过一般不用这种方式）
	//glBindTexture(GL_TEXTURE_2D, 0); //解绑Texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourTextureAppend, 0);

	//绑定渲染缓冲对象 renderBufferObject
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//此处就可以利用渲染缓冲对象创建一个深度与模板缓冲附件   渲染缓冲对象只写不读（除非使用glReadPixels方法读取帧缓冲，否则只支持写入，很适合深度与模板信息）
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

		//std::cout << "移动距离 " << (1.0 + sin(glfwGetTime()) / 2.0 + 0.5) << std::endl;
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
			std::cout << "准备输出图片" << std::endl;
			imageData = new unsigned char[4 * width * height];
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
			// OPEN_GL在前颜色缓冲区中渲染 | GL_BACK 在后颜色缓冲区中渲染
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

/* BMP图片保存 支持RGB数据，木有A哈 */
void saveToImage(unsigned char* data,int width,int height) {
	const int colorBufferSize = width * height * sizeof(char) * 3;

	std::cout << "wdith = " << width << " , height = " << height << "" << std::endl;
	
	BitmapFileHeader fileHeader;
	fileHeader.bfType = 0X4D42; // BM的16进制
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

	//	std::cout << "鼠标输入" << std::endl;

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


	std::cout << "修改前 ： x = " << cameraLightPos.x << " , y = " << cameraLightPos.y << " , z = " << cameraLightPos.z << std::endl;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		std::cout << "按键W" << std::endl;
		cameraLightPos += cameraSpeed * cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		std::cout << "按键S" << std::endl;
		saveCount++;
		cameraLightPos -= cameraSpeed * cameraLightFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		std::cout << "按键A" << std::endl;
		cameraLightPos -= glm::normalize(glm::cross(cameraLightFront, cameraLightUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		std::cout << "按键D" << std::endl;
		cameraLightPos += glm::normalize(glm::cross(cameraLightFront, cameraLightUp)) * cameraSpeed;
	}

	std::cout << "修改后 ： x = " << cameraLightPos.x << " , y = " << cameraLightPos.y << " , z = " << cameraLightPos.z << std::endl;

}