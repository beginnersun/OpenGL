#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

//��ǰ�����STB_IMAGE_TMPLEMENTATION ��stb_image.h��������������


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

	if (glewInit() != GLEW_OK) //��ʼ��Glew��  ���������ר��������̬��ȡOpenGl������ַ�ģ���ΪOpenGl�ĺ�����ַ������ʱ��ѯ�������ڼ��޷�ȷ����
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

	ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");//���ļ����浽��ĿĿ¼�£�Ȼ���ȡ

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	glUseProgram(shader);

//	glBindVertexArray(VAO);
	/*unsigned int shader = CreateShader(vertexShader, fragmentShader);
	glUseProgram(shader);*/


	unsigned int texture;
	glGenTextures(1, &texture);
	//GL_TEXTURE0 ΪĬ�ϵļ��������Ԫ
	//�����֧�ֶ�����Ե���glActiveTexture�󶨶������Ԫ
	//Ȼ���Ӧ��Ƭ����ɫ���н�������һ��������
	//glActiveTexture(GL_TEXTURE0);
	//������: ���ǽ����صļ��ص��������ݴ�����ɫ������ô���룿ͨ��in?�������ꣿ�������ꣿ
	//����������ͨ��uniformȻ��ͨ��GLSL�ṩ���������ʹ�õ��ڽ��������ͣ�Sampler��������
	//���������� ����Sampler����ͨ��uniform��һ�� ����Ҫͨ��glUniform�����������
	//��������ִ��glDrawElementsʱ���Զ�������ֵ��ֵ����ɫ���Ĳ�������
	//��ΪglVertexAttribPointer�������������ֵ
	glBindTexture(GL_TEXTURE_2D, texture);

	stbi_set_flip_vertically_on_load(true);

	int nrChannels;
	unsigned char *data = stbi_load("res/image/container.jpg", &width, &height, &nrChannels, 0);

	std::cout << "���ݴ�С" << sizeof(data) << std::endl;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //���������ص����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//ģ��Ȼ���ƽ��
	
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	//��һ��0ָ���˶༶��Զ����ļ���    һ����0  ��������
	//�ڶ���0Ϊ��ʷ��������
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);//�ͷ�����


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

	stbi_image_free(data);//�ͷ�����

	glUseProgram(shader);

	glUniform1i(glGetUniformLocation(shader,"ourTexture1"),0);
	glUniform1i(glGetUniformLocation(shader, "ourTexture2"), 1);


	/*glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);

	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 1.0f));
	vec = trans * vec;

	std::cout << vec.x << vec.y << vec.z;*/

	//��������ͶӰ���൱�ڴ���һ�������壬Ȼ�������ڵ��ڴ��������ڵ�������ʾ �� ��������֮���ȫ���ü�����
	//ǰ��������Ϊ��������  ����Ϊƽ��ͷ��ײ��Ͷ���  ����Ϊ��ƽ����Զƽ������
	//���������������������� �ײ��Ͷ����������������� Ȼ����������ǰ������ ��ϳ�һ��������
	glm::ortho(0.0f,800.0f,0.0f,600.0f,0.1f,100.0f);


	//����һ��͸��ͶӰ  ͸��ͶӰ������ͶӰ������Ϊ͸��ͶӰ��������Ϊ���ۿ���Ч��
	//ԽԶ�ĵط���ԽС ����ƽ����ԽԶ����ȥ�ͻ��𽥡��ཻ����͸��ͶӰ����һ������̨��
	//��һ��������һ��fov��ֵ Field of View(��Ұ),���������˹۲�ռ�Ĵ�С��
	//45.0f��һ���Ƚ���ʵ��Ч��  �����Ҫ���������Ե�����С
	//�ڶ������������˿�߱�  �ӿڵĿ�߱ȣ�����̨С��һ��Ŀ�߱ȣ�
	//�����ĸ����������˽�ƽ����Զƽ��
	glm::perspective(glm::radians(45.0f), (float)width/(float)height,0.1f,100.0f);


	//����һ��ģ�;��� ������λ�� ���� ����ת�Ĳ���
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model,glm::radians(-55.0f),glm::vec3(1.0f,1.0f,0.0f));

	//����һ���۲����
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
		//��z����ת90��
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

//ע�͵���Basic.shader
//
