#include "config.h"
#include "TextureResource.h"

void TextureResource::loadFromFile(const char* fileName){
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	data = stbi_load(fileName, &width, &height, &comp, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

void TextureResource::genTexture(){
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureResource::bindTexture(){
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void TextureResource::unbindTexture(){
	glBindTexture(GL_TEXTURE_2D, 0);
}
