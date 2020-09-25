#pragma once
#include "MeshResource.h"
#include "TextureResource.h"
#include "ShaderObject.h"

class GraphicsNode {
	std::shared_ptr<MeshResource> mesh;
	std::shared_ptr<TextureResource> texture;
	std::shared_ptr<ShaderObject> shader;
	Matrix4 transform;
	int vertices;

public:
	void setMesh(MeshResource& mesh);
	void setMesh(MeshResource::Shape shape, const int (&elementsPerVertex)[2], float size);
	void setTexture(TextureResource& texture);
	void setTexture(std::string textureFile);
	void setShader(ShaderObject& shader);
	void setShader(std::string vertexFile, std::string fragmentFile);
	void setTransform(Matrix4 newTransform);

	MeshResource& getMesh();
	TextureResource& getTexture();
	ShaderObject& getShader();
	Matrix4& getTransform();

	void draw();

	void destroyAll();
};