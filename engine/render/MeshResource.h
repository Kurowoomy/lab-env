#pragma once

#include "GL/glew.h"
#include <vector>
#include "core/Math_Library.h"

/// Contains ID for 1 vertexBuffer, 1 indexBuffer, 1 vertexArray
class MeshResource {
public:
	enum class Shape {
		SQUARE,
		CUBE
	};

	GLuint vertexID;
	GLuint indexID;
	GLuint vertexArrayID;
	GLuint layouts, offset; // used in addArrayAttribute
	std::vector<Vec3> vertices, normals;
	std::vector<Vec2> uvs;

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
	
	void generateSquare(GLfloat size);
	void generateCube(GLfloat size);


	/// Adds a new float layout for vertex shader and increments layouts by one. 
	/// Uses stride, updates offset used for next layout.
	void addArrayAttribute(GLuint elementsPerVertex);

	void destroyID();
	~MeshResource();
};