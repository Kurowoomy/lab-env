#pragma once

#include "core/Math_Library.h"
#include "GL/glew.h"
#include <vector>


/// Contains 1 vertexBuffer, 1 indexBuffer, 1 vertexArray
class MeshResource {
public:
	GLuint vertexID;
	GLuint indexID;
	GLuint vertexArrayID;
	GLuint count;
	GLuint stride;
	GLuint layouts;
	GLuint offset;

	void vertexBind();
	void indexBind();
	void vertexArrayBind();
	void vertexUnbind();
	void indexUnbind();
	void vertexArrayUnbind();

	void genVertexBuffer(const GLfloat* buf, GLuint size, const GLuint stride);
	void genIndexBuffer(const GLuint* ibuf, GLuint count);
	void genVertexArray();

	void addArrayAttribute(GLuint elementsPerVertex);

	MeshResource();
	~MeshResource();
	
};