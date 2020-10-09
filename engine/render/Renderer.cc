#include "config.h"
#include "Renderer.h"

void* Renderer::addVertexIndexBuffer(const char* objPath) {
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
		Vec3 normal = tempNormals[normalIndices[i] - 1];
		mesh.get()->normals.push_back(normal);
	}
	std::vector<unsigned int> indexBuffer;
	for (int i = 0; i < vertexIndices.size(); i++) {
		indexBuffer.push_back(i);
	}
	// -----------------------------------------------------------------

	// TODO: upload buffers to gpu

	buffers.vertexBuffer = mesh.get()->vertices;
	buffers.indexBuffer = indexBuffer;
	return &buffers;
}

void Renderer::setFramebuffer(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);

}
unsigned int Renderer::getFramebufferWidth() {
	return framebuffer.width;
}
unsigned int Renderer::getFramebufferHeight() {
	return framebuffer.height;
}