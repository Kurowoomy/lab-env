#pragma once

#include "GL/glew.h"

/// Contains ID for 1 vertexBuffer, 1 indexBuffer, 1 vertexArray
class MeshResource {
public:
	GLuint vertexID;
	GLuint indexID;
	GLuint vertexArrayID;
	GLuint count;
	GLuint stride;
	GLuint layouts;
	GLuint offset;

	MeshResource();

	/// Only one line per bind function. (glBind*)
	void vertexBind();
	void indexBind();
	void vertexArrayBind();
	/// Only one line per unbind function. (glUnbind*)
	void vertexUnbind();
	void indexUnbind();
	void vertexArrayUnbind();

	/// Assigns ID to vertexArrayID. Generates vertex array for binding vertex and index buffer in it.
	void genVertexArray();
	/// Saves vertex buffer data in GL_ARRAY_BUFFER, assigns an ID numbervertexID. Sets stride.
	void genVertexBuffer(const GLfloat* buf, GLuint size, const GLuint stride);
	/// Saves index buffer data in GL_ELEMENT_ARRAY_BUFFER, assigns an ID number to indexID.
	void genIndexBuffer(const GLuint* ibuf, GLuint count);

	/// Adds a new float layout for vertex shader and increments layouts by one. 
	/// Uses stride, updates offset used for next layout.
	void addArrayAttribute(GLuint elementsPerVertex);

	~MeshResource();
};