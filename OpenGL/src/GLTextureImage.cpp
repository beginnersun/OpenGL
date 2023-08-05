#include "GLTextureImage.h"
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

GLTextureImage::GLTextureImage(const char* path) {
	this->path = path;
	glGenTextures(1, &this->textureId);
	this->loadImage();
}

void GLTextureImage::loadImage(){
	int nrChannels;
	int width, height;
	unsigned char *data = stbi_load(this->path,&width,&height,&nrChannels,0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glBindTexture(GL_TEXTURE_2D,this->textureId);

		glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
		//ʹ�ö༶��Զ�������Ҫ�ֶ����ý�����������ʹ��glGenerateMipmap����
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at Path : " << this->path << std::endl;
		stbi_image_free(data);
	}
}