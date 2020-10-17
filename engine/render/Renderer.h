#pragma once

#include "render/MeshResource.h"
#include <fstream>
#include <functional>
#include "render/stb_image.h"
#include <algorithm>


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
	std::vector<std::vector<Vec4>> colorBuffer; // [y][x], [height][width]
	unsigned int width, height;
};

class Renderer {
public:
	Buffers buffers;
	MeshResource mesh;
	Framebuffer framebuffer;
	GLuint framebufferID, colorID, depthID;
	std::function<Vec4(Vertex&)> vertexShader;
	void(*fragmentShader)(Vec3 v3);
	Matrix4 model;
	Vec3 worldPos;
	unsigned char* textureColor;
	int textureWidth, textureHeight, channels;
	std::vector<std::vector<Vec3>> normals; // nya/tomma f�r varje triangel [y][x]
	std::vector<std::vector<Vec2>> uvCoords; // nya/tomma f�r varje triangel [y][x]
	std::vector<std::vector<int>> pixels;

	// Reads obj file, saves all data to mesh and buffers in this Renderer object.
	void* addVertexIndexBuffer(const char* objPath);

	void setFramebuffer(unsigned int width, unsigned int height);
	unsigned int getFramebufferWidth();
	unsigned int getFramebufferHeight();
	void bindFramebuffer();
	void unbindFramebuffer();

	void rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2);
	void draw(void* buffers);

	void setVertexShader(const std::function<Vec4(Vertex&)> vertexShader);
	void setFragmentShader(void(*fragmentShader)(Vec3));

	void loadTextureFile(const char* fileName);

	std::vector<Vec2> createLine(int x0, int x1, int y0, int y1);
	Vec3 convertToRasterSpace(Vec4& v);
	void fillTriangle(std::vector<Vec2> line0, std::vector<Vec2> line1, std::vector<Vec2> line2);
};