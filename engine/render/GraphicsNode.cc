#include "config.h"
#include "GraphicsNode.h"

void GraphicsNode::setMesh(MeshResource& mesh)
{
	this->mesh = std::make_shared<MeshResource>(mesh);
}
void GraphicsNode::setMesh(MeshResource::Shape shape, const int (&elementsPerVertex)[2], float size)
{
	mesh = std::make_shared<MeshResource>();
	mesh.get()->genVertexArray();

	switch (shape) {
	case MeshResource::Shape::SQUARE:
		mesh.get()->generateSquare(size);
		vertices = 6;
		break;
	case MeshResource::Shape::CUBE:
		mesh.get()->generateCube(size);
		vertices = 36;
		break;
	}
	for (int value : elementsPerVertex) {
		mesh.get()->addArrayAttribute(value);
	}

	mesh.get()->vertexArrayUnbind();
	mesh.get()->vertexUnbind();
	mesh.get()->indexUnbind();
}
void GraphicsNode::setTexture(TextureResource& texture)
{
	this->texture = std::make_shared<TextureResource>(texture);
}
void GraphicsNode::setTexture(std::string textureFile)
{
	texture = std::make_shared<TextureResource>();
	texture.get()->loadFromFile(textureFile.c_str());
}
void GraphicsNode::setShader(ShaderObject& shader)
{
	this->shader = std::make_shared<ShaderObject>(shader);
}
void GraphicsNode::setShader(std::string vertexFile, std::string fragmentFile) 
{
	shader = std::make_shared<ShaderObject>();
	shader.get()->generateVertexShader(vertexFile.c_str());
	shader.get()->generateFragmentShader(fragmentFile.c_str());
	shader.get()->generateProgram();
}
void GraphicsNode::setTransform(Matrix4 newTransform) 
{
	transform = newTransform;
}

MeshResource& GraphicsNode::getMesh()
{
	return *mesh;
}
TextureResource& GraphicsNode::getTexture()
{
	return *texture;
}
ShaderObject& GraphicsNode::getShader()
{
	return *shader;
}
Matrix4& GraphicsNode::getTransform() 
{
	return transform;
}

void GraphicsNode::draw()
{
	getTexture().bindTexture();
	getMesh().vertexArrayBind();
	glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);
	getMesh().vertexArrayUnbind();
	getTexture().unbindTexture();
	getShader().quitProgram();
}

void GraphicsNode::destroyAll()
{
	getShader().destroy();
	getTexture().destroyID();
	getMesh().destroyID();
}
