#pragma once

#include "render/MeshResource.h"
#include <fstream>

struct Buffers {
	std::vector<Vec3> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
};
struct Framebuffer {
	std::vector<Vec3> colorBuffer;
	unsigned int width, height;
};
struct Vertex {
	Vec3 pos;
	Vec3 normal;
	Vec2 uv;
};

class Renderer {
public:
	Buffers buffers;
	std::shared_ptr<MeshResource> mesh;
	Framebuffer framebuffer;
	GLuint framebufferID, colorID, depthID;

	// Reads obj file, saves all data to mesh and buffers in this Renderer object.
	void* addVertexIndexBuffer(const char* objPath);

	void setFramebuffer(unsigned int width, unsigned int height);
	unsigned int getFramebufferWidth();
	unsigned int getFramebufferHeight();
	void bindFramebuffer();
	void unbindFramebuffer();

	void rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2);
};