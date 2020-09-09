#pragma once

#include "core/Math_Library.h"
#include "GL/glew.h"

//class VertexBuffer {
//public:
//	GLuint rendererID;
//	VertexBuffer();
//
//	void genVertexBuffer(const GLfloat* buf, GLuint size);
//	void Bind() const;
//	void Unbind() const;
//	
//	~VertexBuffer();
//};
//
//class IndexBuffer {
//public:
//	GLuint rendererID;
//	GLuint count;
//	IndexBuffer();
//
//	void genIndexBuffer(const GLuint* data, GLuint size);
//	void Bind() const;
//	void Unbind() const;
//
//	~IndexBuffer();
//};

class MeshResource {
public:
	GLuint vertexID;
	GLuint indexID;
	GLuint vertexArrayID;
	GLuint count;

	void vertexBind();
	void indexBind();
	void vertexArrayBind();
	void vertexUnbind();
	void indexUnbind();
	void vertexArrayUnbind();
	void genVertexBuffer(const GLfloat* buf, GLuint size);
	void genIndexBuffer(const GLuint* ibuf, GLuint count);
	void genVertexArray();

	MeshResource();
	~MeshResource();
	// vertex layout attributes here
	
};