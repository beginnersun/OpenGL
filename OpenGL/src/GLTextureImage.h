#pragma once

#include <stb_image.h>

class GLTextureImage
{
public:

	unsigned int textureId;

	GLTextureImage(const char * path);

private:

	const char * path;

	void loadImage();

};

