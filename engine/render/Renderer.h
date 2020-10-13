#pragma once

#include "render/MeshResource.h"
#include <fstream>
#include <functional>


struct Vertex {
	Vertex(Vec3 pos, Vec3 normal, Vec2 uv) : pos(pos), normal(normal), uv(uv) {}
	Vec3 pos;
	Vec3 normal;
	Vec2 uv;
};
struct Buffers {
	std::vector<Vertex> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
};
struct Framebuffer {
	std::vector<Vec4> colorBuffer;
	unsigned int width, height;
};

class Renderer {
public:
	Buffers buffers;
	MeshResource mesh;
	Framebuffer framebuffer;
	GLuint framebufferID, colorID, depthID;
	std::function<void(Vertex)> vertexShader;
	void(*fragmentShader)(Vec3 v3);
	Matrix4 model;
	Vec3 worldPos;

	// Reads obj file, saves all data to mesh and buffers in this Renderer object.
	void* addVertexIndexBuffer(const char* objPath);

	void setFramebuffer(unsigned int width, unsigned int height);
	unsigned int getFramebufferWidth();
	unsigned int getFramebufferHeight();
	void bindFramebuffer();
	void unbindFramebuffer();

	void rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2);
	void draw(void* buffers);

	void setVertexShader(const std::function<void(Vertex)> vertexShader);
	void setFragmentShader(void(*fragmentShader)(Vec3));
};