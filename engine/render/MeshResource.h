#pragma once

#include "core/Math_Library.h"
#include "GL/glew.h"

class VertexBuffer {
	GLuint rendererID;
public:
	VertexBuffer(const void* data, GLuint size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};

class IndexBuffer {
	GLuint rendererID;
	GLuint count;
public:
	IndexBuffer(const GLuint* data, GLuint count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
};

class MeshResource {
public:
	VertexBuffer vb;
	IndexBuffer ib;

	MeshResource();
	~MeshResource();
	
};