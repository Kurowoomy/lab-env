#pragma once

#include "render/MeshResource.h"
#include <functional>
#include "render/stb_image.h"
#include "TextureResource.h"
#include <map>


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
	std::vector<float> depthBuffer;
	unsigned int width, height;
};

class Renderer {
public:
	Buffers buffers; // Fill this buffer in addVertexIndexBuffer if it's empty.
	Framebuffer framebuffer; // has depthBuffer and colorBuffer
	GLuint framebufferID;
	std::function<Vec4(Vertex&)> vertexShader;
	std::function<Vec4(Vec2, Vec3, unsigned char*)> pixelShader;

	Matrix4 model, viewMatrix, projectionMatrix;
	Vec3 cameraPos, worldPos;
	float near = 0.1, far = 3000;
	
	// reset these three for every triangle rasterization
	std::vector<Vec3> normals;
	std::vector<Vec2> uvCoords;
	std::vector<Vec3> pixels;

	TextureResource texture;

	// Reads obj file, saves all data to mesh and buffers in this Renderer object.
	void* addVertexIndexBuffer(const char* objPath);

	// framebuffer, shader, texture setup
	void setFramebuffer(unsigned int width, unsigned int height);
	void bindFramebuffer();
	void unbindFramebuffer();
	void setVertexShader(const std::function<Vec4(Vertex&)> vertexShader);
	void setPixelShader(const std::function<Vec4(Vec2, Vec3, unsigned char*)> pixelShader);
	void loadTextureFile(const char* fileName);

	// rasterizing
	Vec3 convertToRasterSpace(Vec4& v);
	std::vector<Vec2> createLine(int x0, int x1, int y0, int y1);
	void drawLine(std::vector<Vec2> line);
	void fillTriangle(std::vector<Vec2> line0, std::vector<Vec2> line1, std::vector<Vec2> line2);
	void fillRow(int x0, int x1, int y);
	void interpolate(int x, int y, int i, Vertex& v0, Vertex& v1, Vertex& v2, Vec3 pos0, Vec3 pos1, Vec3 pos2);
	
	// draw frame
	void rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2);
	void draw(void* buffers);

	// math
	float min(float a, float b);
	float max(float a, float b);
	float pow(float a, float b);
	void setTransform(Matrix4 newTransform);
	Matrix4& getTransform();
};