#pragma once
class GLTextureImage
{
public:

	unsigned int textureId;

	GLTextureImage(const char * path);

private:

	const char * path;

	void loadImage();

};

