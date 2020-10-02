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
void GraphicsNode::setMesh(const char* objPath)
{
	// read objFile
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vec3> tempVertices, tempNormals;
	std::vector<Vec2> tempUvs;
	FILE* objFile = fopen(objPath, "r");
	if (objFile == NULL) {
		printf("Impossible to open the obj file !\n");
	}
	while (true) {
		char firstWord[128] = { 0 };
		int readBytes = fscanf(objFile, "%s", firstWord);
		if (readBytes == EOF) {
			break;
		}
		if (strcmp(firstWord, "v") == 0) {
			Vec3 vertex;
			if (!fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z)) {
				printf("Can't read v line\n");
			}
			tempVertices.push_back(vertex);
		}
		else if (strcmp(firstWord, "vt") == 0) {
			Vec2 uv;
			if (!fscanf(objFile, "%f %f\n", &uv.x, &uv.y)) {
				printf("Can't read vt line\n");
			}
			tempUvs.push_back(uv);
		}
		else if (strcmp(firstWord, "vn") == 0) {
			Vec3 normal;
			if (!fscanf(objFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z)) {
				printf("Can't read vn line\n");
			}
			tempNormals.push_back(normal);
		}
		else if (strcmp(firstWord, "f") == 0) {
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :( Try exporting with other options\n");
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	// make mesh
	mesh = std::make_shared<MeshResource>();
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		Vec3 vertex = tempVertices[vertexIndices[i] - 1];
		mesh.get()->vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < uvIndices.size(); i++) {
		Vec2 uv = tempUvs[uvIndices[i] - 1];
		uv.y = 1 - uv.y;
		mesh.get()->uvs.push_back(uv);
	}
	for (unsigned int i = 0; i < normalIndices.size(); i++) {
		unsigned int normalIndex = normalIndices[i];
		Vec3 normal = tempNormals[normalIndex - 1];
		mesh.get()->normals.push_back(normal);
	}
	std::vector<unsigned int> indexBuffer;
	for (int i = 0; i < vertexIndices.size(); i++) {
		indexBuffer.push_back(i);
	}
	// -----------------------------------------------------------------

	// transfer to GPU
	mesh.get()->genVertexArray();

	glGenBuffers(1, &mesh.get()->vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.get()->vertexID);
	glBufferData(GL_ARRAY_BUFFER, mesh.get()->vertices.size() * sizeof(Vec3), &mesh.get()->vertices[0], GL_STATIC_DRAW);
	mesh.get()->addArrayAttribute(3, 3); // vertices
	mesh.get()->vertexUnbind();

	//glGenBuffers(1, &mesh.get()->textureID);
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.get()->textureID);
	//glBufferData(GL_ARRAY_BUFFER, mesh.get()->uvs.size() * sizeof(Vec3), &mesh.get()->uvs[0], GL_STATIC_DRAW);
	//mesh.get()->addArrayAttribute(2, 2); // uvs
	//mesh.get()->vertexUnbind();

	glGenBuffers(1, &mesh.get()->indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.get()->indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	mesh.get()->vertexArrayUnbind();
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
	//getTexture().bindTexture();
	getMesh().vertexArrayBind();
	//glDrawElements(GL_TRIANGLES, vertices, GL_UNSIGNED_INT, nullptr);
	glDrawElements(GL_TRIANGLES, mesh.get()->vertices.size(), GL_UNSIGNED_INT, nullptr);
	getMesh().vertexArrayUnbind();
	//getTexture().unbindTexture();
	getShader().quitProgram();
}

void GraphicsNode::destroyAll()
{
	getShader().destroy();
	//getTexture().destroyID();
	getMesh().destroyID();
}
