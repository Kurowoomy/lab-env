#include "config.h"
#include "Renderer.h"



void* Renderer::addVertexIndexBuffer(const char* objPath)
{
	if (buffers.indexBuffer.empty()) {
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
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			Vec3 vertex = tempVertices[vertexIndices[i] - 1];
			mesh.vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			Vec2 uv = tempUvs[uvIndices[i] - 1];
			uv.y = 1 - uv.y;
			mesh.uvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			Vec3 normal = tempNormals[normalIndices[i] - 1];
			mesh.normals.push_back(normal);
		}
		for (int i = 0; i < vertexIndices.size(); i++) {
			// write all values into the buffers in this renderer class
			buffers.indexBuffer.push_back(i);
			Vertex v = Vertex(mesh.vertices[i], mesh.normals[i], mesh.uvs[i]);
			buffers.vertexBuffer.push_back(v);
		}
	}

	return &buffers;
}

void Renderer::setFramebuffer(unsigned int width, unsigned int height) 
{
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);

	framebuffer.width = width;
	framebuffer.height = height;
	framebuffer.colorBuffer.resize(height);
	for (int i = 0; i < framebuffer.colorBuffer.size(); i++) {
		framebuffer.colorBuffer[i].resize(width);
	}
	
	glGenTextures(1, &colorID);
	glBindTexture(GL_TEXTURE_2D, colorID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("D: Framebuffer incomplete.");
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
unsigned int Renderer::getFramebufferWidth() {
	return framebuffer.width;
}
unsigned int Renderer::getFramebufferHeight() {
	return framebuffer.height;
}
void Renderer::bindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
}
void Renderer::unbindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2) 
{
	// omvandling av vertiserna så de befinner sig i rätt position i förhållande till worldspace
	vertexShader(v0);
	vertexShader(v1);
	vertexShader(v2);
	// -----------------------------------------------------------------------------------------

	// Måste först hitta alla pixlar man behöver jobba med för denna triangel-------------------
	// dvs bresenhams algorithm och scanline
	normals.clear();
	uvCoords.clear();





	// TODO: använd v0, v1, v2 för att få alla mellanvärden för normals och uvcoords.
	// dvs interpolera.


	// -----------------------------------------------------------------------------------------

	// pixel shader ----------------------------------------------------------------------------
	// TODO: använd pixelShader-metoden här. och textureColor. [i], [i+1], [i+2], [i+3]  = r, g, b, a.
	// textureColor = objectColor. 

	// -----------------------------------------------------------------------------------------
}

void Renderer::draw(void* handle) 
{
	Buffers* buffers = (Buffers*)handle;
	
	for (int i = 0; i < buffers->indexBuffer.size(); i+=3) {
		rasterizeTriangle(buffers->vertexBuffer[i], buffers->vertexBuffer[i + 1], buffers->vertexBuffer[i + 2]);
	}
}

void Renderer::setVertexShader(const std::function<void(Vertex)> vertexShader) {
	this->vertexShader = vertexShader;
}
void Renderer::setFragmentShader(void(*fragmentShader)(Vec3)) {
	this->fragmentShader = fragmentShader;
}

void Renderer::loadTextureFile(const char* fileName) {
	textureColor = stbi_load(fileName, &textureWidth, &textureHeight, &channels, 0);
}

void bresenham(Vertex p0, Vertex p1) 
{

}