#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//提前定义好STB_IMAGE_TMPLEMENTATION 将stb_image.h里面代码引入进来


#define STB_IMAGE_IMPLEMENTATION

float mixValue = 0.2f;

#include <stb_image.h>



struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

//void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(stream,line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(),ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")  << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER,fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

int macdin(void)
{
	GLFWwindow* window;
	 
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(600, 600, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);

	std::cout << "width = " << width << " , height = " << height << std::endl;
	glViewport(0, 0, 600, 600);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) //初始化Glew库  （这个库是专门用来动态获取OpenGl函数地址的）因为OpenGl的函数地址是运行时查询（编译期间无法确定）
	{
		std::cout << "Error !" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	/*float positions[12] = {
		-0.5f,-0.5f,
		0.5f,-0.5f,
		0.5f,0.5f,

		-0.5f,-0.5f,
		-0.5f,0.5f,
		0.5f,0.5f
	};*/


	float positions[28] = {
		0.5f,0.5f, 1.0f,0.0f,0.0f,  1.0f , 1.0f,
		0.5f,-0.5f, 0.0f, 1.0f, 0.0f, 1.0f , 0.0f,
		-0.5f,-0.5f, 0.0f, 0.0f ,1.0f,  0.0f , 0.0f,
		-0.5f,0.5f, 1.0f, 0.0f ,1.0f, 0.0f , 1.0f
	};


	unsigned int indices[6] = {
		0,1,2,
		2,3,0
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);


	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	

	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;"
		"\n"
		"layout(location = 1) in vec4 aColor;"
		"\n"
		"out vec4 ourColor;"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"	ourColor = aColor;"
		"}\n";
	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;"
		"\n"
		"in vec4 ourColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = ourColor;\n"
		"}\n";

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");//将文件保存到项目目录下，然后读取

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

//	glBindVertexArray(VAO);
	/*unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);*/


	unsigned int texture;
	glGenTextures(1, &texture);
	//GL_TEXTURE0 为默认的激活的纹理单元
	//如果想支持多个可以调用glActiveTexture绑定多个纹理单元
	//然后对应的片段着色器中接收另外一个采样器
	//glActiveTexture(GL_TEXTURE0);
	//采样器: 我们将本地的加载的纹理数据传入着色器，怎么传入？通过in?纹理坐标？顶点坐标？
	//都不是我们通过uniform然后通过GLSL提供的纹理对象使用的内建数据类型（Sampler）采样器
	//来保存数据 但是Sampler与普通的uniform不一样 不需要通过glUniform这个方法传递
	//参数，在执行glDrawElements时会自动把纹理值赋值给着色器的采样器。
	//因为glVertexAttribPointer中有设置纹理的值
	glBindTexture(GL_TEXTURE_2D, texture);

	stbi_set_flip_vertically_on_load(true);

	int nrChannels;
	unsigned char *data = stbi_load("res/image/container.jpg", &width, &height, &nrChannels, 0);

	std::cout << "数据大小" << sizeof(data) << std::endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //亮但是像素点分明
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//模糊然后变平滑
	
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	//第一个0指定了多级渐远纹理的级别    一般填0  基本级别
	//第二个0为历史遗留问题
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);//释放数据


	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);


	data = stbi_load("res/image/awesomeface.png",&width,&height,&nrChannels,0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);//释放数据

	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader,"ourTexture1"),0);
	glUniform1i(glGetUniformLocation(shader, "ourTexture2"), 1);


	/*glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);

	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 1.0f));
	vec = trans * vec;

	std::cout << vec.x << vec.y << vec.z;*/

	//创建正视投影（相当于创建一个立方体，然后将世界内的在此立方体内的坐标显示 ， 此立方体之外的全部裁减掉）
	//前两个坐标为左右坐标  三四为平截头体底部和顶部  五六为近平面与远平面坐标
	//左右坐标限制左右两个面 底部和顶部限制上下两个面 然后五六限制前后两面 组合出一个立方体
	glm::ortho(0.0f,800.0f,0.0f,600.0f,0.1f,100.0f);


	//创建一个透视投影  透视投影与正视投影的区别为透视投影可以想象为人眼看的效果
	//越远的地方会越小 两条平行线越远看过去就会逐渐“相交”（透视投影像是一个四棱台）
	//第一个参数定一了fov的值 Field of View(视野),并且设置了观察空间的大小。
	//45.0f是一个比较真实的效果  如果想要其他风格可以调整大小
	//第二个参数设置了宽高比  视口的宽高比（四棱台小的一面的宽高比）
	//第三四个参数设置了近平面与远平面
	glm::perspective(glm::radians(45.0f), (float)width/(float)height,0.1f,100.0f);


	//创建一个模型矩阵 包含了位移 缩放 与旋转的操作
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model,glm::radians(-55.0f),glm::vec3(1.0f,1.0f,0.0f));

	//创建一个观察矩阵
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 600.0f / 600.0f, 0.1f, 100.0f);



	/* Loop until the user closes the window */
	glfwSetKeyCallback(window, key_callback);
	while (!glfwWindowShouldClose(window))
	{
		//processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
		float timeValue = glfwGetTime();
		float greenValue = sin(timeValue) / 2.0 + 0.5f;
		int vertexColorLocation = glGetUniformLocation(shader, "ourColor");
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

		float x_offset = sin(timeValue);
		float y_offset = cos(timeValue);
		/*int vertexOffsetLocation = glGetUniformLocation(shader, "p_offset");
		glUniform3f(vertexOffsetLocation, x_offset, y_offset, 0.0f);
		*/

		int mixValueLocation = glGetUniformLocation(shader,"mixValue");
		glUniform1f(mixValue, mixValue);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);


		/*glm::mat4 trans = glm::mat4(1.0f);
		//绕z轴旋转90度
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));*/

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		trans = glm::translate(trans,glm::vec3(0.5f,-0.5f,0.0f));

		unsigned int transformLoc = glGetUniformLocation(shader, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);

		trans = glm::mat4(1.0f); // reset it to identity matrix
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		float scaleAmount = static_cast<float>(sin(glfwGetTime()));
		trans = glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &trans[0][0]); // th

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		/*
		//glBindTexture(GL_TEXTURE_2D, texture);
		//glBindVertexArray(VAO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		*/

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &ibo);
	glDeleteTextures(1, &texture);
	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
/*
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue >= 1.0f)
			mixValue = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
		if (mixValue <= 0.0f)
			mixValue = 0.0f;
	}
}*/

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//注释掉的Basic.shader
//
