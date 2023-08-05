#pragma once
#include <stdio.h>
#include <string>
#include <glm.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>


class Shader {
public:
	unsigned int ID;
	Shader(const char *vertexPath, const char *fragmentPath);

	void use();

	void setBool(const std::string &name, bool value) const;

	void setInt(const std::string &name, int value) const;

	void setFloat(const std::string &name, float value) const;

	void setVec3(const std::string &name, glm::vec3 value) const;

	void setVec3(const std::string &name, float x, float y, float z) const;

	void setMat4(const std::string &name, glm::mat4 value) const;

private:
	void checkCompileErrors(unsigned int shader, std::string type);
};
