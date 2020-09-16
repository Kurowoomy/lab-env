#pragma once

#include "render/stb_image.h"
#include "GL/glew.h"

class TextureResource {
public:
	GLuint textureID;
	GLubyte* data;
	int width, height, comp;
	
	void loadFromFile(const char* fileName);
	void genTexture();
	void bindTexture();
	void unbindTexture();
};