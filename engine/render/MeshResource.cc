#include "config.h"
#include "MeshResource.h"

MeshResource::MeshResource() {
	vertexID = 0;
	indexID = 0;
	vertexArrayID = 0;
	layouts = 0;
	stride = 0;
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
	stride = 5;
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
	stride = 5;
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	GLfloat buf[] =
	{
		// face 1 (kanske z måste vara omvänt?)
		-0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 0
		0, 1,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 1
		0, 0,
		0.5f * size,	0.5f * size,	-0.5f * size,			// pos 2
		1, 0,
		0.5f * size,	-0.5f * size,	-0.5f * size,
		1, 1,
		// face 2 TODO: fix uv coordinates
		-0.5f * size,	-0.5f * size,	0.5f * size,			// pos 0
		0, 1,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	0.5f * size,			// pos 1
		0, 0,
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 2
		1, 0,
		-0.5f * size,	-0.5f * size,	-0.5f * size,
		1, 1
		// face 3
		-0.5f * size,	-0.5f * size,	0.5f * size,			// pos 0
		1, 0,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	0.5f * size,			// pos 1
		1, 1,
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 2
		0, 1,
		0.5f * size,	-0.5f * size,	0.5f * size,
		0, 0,
		// face 4
		0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		0.5f * size,	0.5f * size,	-0.5f * size,			// pos 1
		0, 1,
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		0.5f * size,	-0.5f * size,	0.5f * size,
		1, 0,
		// face 5
		-0.5f * size,	0.5f * size,	-0.5f * size,			// pos 0
		0, 0,													// uv coordinates pos 0
		-0.5f * size,	0.5f * size,	0.5f * size,			// pos 1
		0, 1,
		0.5f * size,	0.5f * size,	0.5f * size,			// pos 2
		1, 1,
		0.5f * size,	0.5f * size,	-0.5f * size,
		1, 0,
		// face 6
		-0.5f * size,	-0.5f * size,	-0.5f * size,			// pos 0
		1, 1,													// uv coordinates pos 0
		-0.5f * size,	-0.5f * size,	0.5f * size,			// pos 1
		1, 0,
		0.5f * size,	-0.5f * size,	0.5f * size,			// pos 2
		0, 0,
		0.5f * size,	-0.5f * size,	-0.5f * size,
		0, 1
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	GLuint ibuf[] =
	{
		// face 1
		0, 1, 2,
		2, 3, 0,
		// face 2 (4, 5, 6, 7)
		4, 5, 6,
		6, 7, 4,
		// face 3 (8, 9, 10, 11)
		11, 10, 9,
		9, 8, 11,
		// face 4 (12, 13, 14, 15)
		12, 13, 14,
		14, 15, 12,
		// face 5 (16, 17, 18, 19)
		16, 17, 18,
		18, 19, 16,
		// face 6 (22, 21, 20, 23)
		22, 21, 20,
		20, 23, 22
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ibuf), ibuf, GL_STATIC_DRAW);
}
//void MeshResource::genVertexBuffer(const GLfloat* buf, GLuint size, const GLuint stride) {
//	this->stride = stride;
//	glGenBuffers(1, &vertexID);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
//	glBufferData(GL_ARRAY_BUFFER, size, buf, GL_STATIC_DRAW);
//}
//void MeshResource::genIndexBuffer(const GLuint* ibuf, GLuint count) {
//	glGenBuffers(1, &indexID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), ibuf, GL_STATIC_DRAW);
//}

void MeshResource::addArrayAttribute(GLuint elementsPerVertex) {
	glEnableVertexAttribArray(layouts);
	glVertexAttribPointer(layouts, elementsPerVertex, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (const void*)offset);
	offset += elementsPerVertex * sizeof(GLfloat);
	layouts++;
}

MeshResource::~MeshResource(){
	glDeleteVertexArrays(1, &vertexArrayID);
}
