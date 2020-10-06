#include "config.h"
#include "MeshResource.h"

MeshResource::MeshResource() {
	vertexID = 0;
	indexID = 0;
	vertexArrayID = 0;
	layouts = 0;
	offset = 0;
}

void MeshResource::vertexBind() {
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
}
void MeshResource::indexBind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
}
void MeshResource::vertexArrayBind() {
	glBindVertexArray(vertexArrayID);
}
void MeshResource::vertexUnbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void MeshResource::indexUnbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void MeshResource::vertexArrayUnbind() {
	glBindVertexArray(0);
}

void MeshResource::genVertexArray() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
}
void MeshResource::generateSquare(GLfloat size) {
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	GLfloat buf[] =
	{
		-0.5f * size,	-0.5f * size,	-0.5 * size,			// pos 0
		0, 1,												// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	-0.5 * size,			// pos 1
		0, 0,
		0.5f * size,	0.5f * size,	-0.5 * size,			// pos 2
		1, 0,
		0.5f * size,	-0.5f * size,	-0.5* size,
		1, 1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	GLuint ibuf[] = //clockwise
	{
		0, 1, 2,
		2, 3, 0
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibuf), ibuf, GL_STATIC_DRAW);
}
void MeshResource::generateCube(GLfloat size) {
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	GLfloat buf[] =
	{ // "rendera 6 olika calls" precis som jag tänker/gjort :D
		// face 1
		-0.5f * size,	0.5f * size,	0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 1
		1, 0,
		0.5f * size,	-0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		-0.5f * size,	-0.5f * size,	0.5f * size,
		0, 1,
		// face 2
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	0.5f * size,			// pos 1
		1, 0,
		-0.5f * size,	-0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		-0.5f * size,	-0.5f * size,	-0.5f * size,
		0, 1,
		// face 3
		0.5f * size,	0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 1
		1, 0,
		-0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 2
		1, 1,
		0.5f * size,	-0.5f * size,	-0.5f * size,
		0, 1,
		// face 4
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		0.5f * size,	0.5f * size,	-0.5f * size,			// pos 1
		1, 0,
		0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 2
		1, 1,
		0.5f * size,	-0.5f * size,	0.5f * size,
		0, 1,
		// face 5
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		0.5f * size,	2 * size,	-0.5f * size,			// pos 1
		1, 0,
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		-0.5f * size,	0.5f * size,	0.5f * size,
		0, 1,
		// face 6
		0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		-0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 1
		1, 0,
		-0.5f * size,	-0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		0.5f * size,	-0.5f * size,	0.5f * size,
		0, 1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	GLuint ibuf[] =
	{
		// face 1 counterclockwise
		0, 1, 2,
		2, 3, 0,
		// face 2 (4, 5, 6, 7)
		4, 5, 6,
		6, 7, 4,
		// face 3 (8, 9, 10, 11)
		8, 9, 10,
		10, 11, 8,
		// face 4 (12, 13, 14, 15)
		12, 13, 14,
		14, 15, 12,
		// face 5 (16, 17, 18, 19)
		16, 17, 18,
		18, 19, 16,
		// face 6 (22, 21, 20, 23)
		20, 21, 22,
		22, 23, 20
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibuf), ibuf, GL_STATIC_DRAW);
}

void MeshResource::addArrayAttribute(GLuint elementsPerVertex) {
	glEnableVertexAttribArray(layouts);
	glVertexAttribPointer(layouts, elementsPerVertex, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (const void*) offset);
	offset += elementsPerVertex * sizeof(GLfloat);
	layouts++;
}
void MeshResource::addArrayAttribute(GLuint elementsPerVertex, GLuint totalSize) {
	glEnableVertexAttribArray(layouts);
	glVertexAttribPointer(layouts, elementsPerVertex, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * totalSize, (const void*)offset);
	offset += elementsPerVertex * sizeof(GLfloat);
	layouts++;
}

void MeshResource::destroyID()
{
	glDeleteBuffers(1, &vertexID);
	glDeleteBuffers(1, &indexID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

MeshResource::~MeshResource(){
}
