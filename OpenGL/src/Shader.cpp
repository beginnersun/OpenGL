#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Shader.h"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		std::cout << "trust open" << vertexPath << std::endl;
		vShaderFile.open(vertexPath);
		std::cout << vertexPath << " file open success!" << std::endl;
		fShaderFile.open(fragmentPath);
		std::cout << fragmentPath << " file open success!" << std::endl;
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl << "error msg = "<< e.what() << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();

	unsigned int vertex = 0, fragment = 0;
	//vertexShader创建

	std::cout << "当前Value" << vertex << std::endl;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	//fragmentShader创建
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);

}
Shader::Shader(const char *vertexPath, const char *gsShaderPath, const char *fragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	bool haveGShader = false;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		std::cout << "trust open" << vertexPath << std::endl;
		vShaderFile.open(vertexPath);
		std::cout << vertexPath << " file open success!" << std::endl;
		gShaderFile.open(gsShaderPath);
		std::cout << gsShaderPath << "file open succes!" << std::endl;
		fShaderFile.open(fragmentPath);
		std::cout << fragmentPath << " file open success!" << std::endl;
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		if (gsShaderPath != NULL)
		{
			haveGShader = true;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl << "error msg = " << e.what() << std::endl;
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	const char *gShaderCode = geometryCode.c_str();

	unsigned int vertex = 0, fragment = 0, gShaer = 0;
	//vertexShader创建

	std::cout << "当前Value" << vertex << std::endl;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	//fragmentShader创建
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	//gShader创建
	if (haveGShader)
	{
		gShaer = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShaer, 1, &gShaderCode, NULL);
		glCompileShader(gShaer);
		checkCompileErrors(gShaer, "GSHADER");
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (haveGShader)
	{
		glAttachShader(ID, gShaer);
	}
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (haveGShader)
	{
		glDeleteShader(gShaer);
	}
}

void Shader::use() {
	    glUseProgram(ID);
}

void Shader::setInt(const std::string &name, int value) const {
	    glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setBool(const std::string &name, bool value) const {
	    glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
}

void Shader::setFloat(const std::string &name, float value) const {
	    glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
	//std::cout << name.c_str() << " 中 setX = " << value.x << " , setY = " << value.y << " , setZ = " << value.z << std::endl;
	glUniform3f(glGetUniformLocation(ID,name.c_str()),value.x,value.y,value.z);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(ID,name.c_str()),x,y,z);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
	glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(value));
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		//glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------- -- " << std::endl;
		}
	}
	else {
		glGetProgramiv(shader,GL_LINK_STATUS,&success);
		if (success == GL_FALSE) {
			glGetProgramInfoLog(shader,1024,NULL,infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------- -- " << std::endl;
		}
	}
}