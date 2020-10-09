#pragma once

#include "render/MeshResource.h"
#include <fstream>

struct Buffers {
	std::vector<Vec3> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
};
struct Framebuffer {
	unsigned int width, height;
};

class Renderer {
public:
	Buffers buffers;
	std::shared_ptr<MeshResource> mesh;
	Framebuffer framebuffer;
	GLuint framebufferID;

	// Reads obj file, saves all data to mesh and buffers in this Renderer object.
	void* addVertexIndexBuffer(const char* objPath);

	void setFramebuffer(unsigned int width, unsigned int height);
	unsigned int getFramebufferWidth();
	unsigned int getFramebufferHeight();
};