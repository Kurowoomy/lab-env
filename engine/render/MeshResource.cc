#include "config.h"
#include "MeshResource.h"

MeshResource::MeshResource() {
	vertexID = 0;
	indexID = 0;
	vertexArrayID = 0;
	count = 0;
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
void MeshResource::genVertexBuffer(const GLfloat* buf, GLuint size, const GLuint stride) {
	this->stride = stride;
	glGenBuffers(1, &vertexID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	glBufferData(GL_ARRAY_BUFFER, size, buf, GL_STATIC_DRAW);
}
void MeshResource::genIndexBuffer(const GLuint* ibuf, GLuint count) {
	this->count = count;
	glGenBuffers(1, &indexID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), ibuf, GL_STATIC_DRAW);
}

void MeshResource::addArrayAttribute(GLuint elementsPerVertex) {
	glEnableVertexAttribArray(layouts);
	glVertexAttribPointer(layouts, elementsPerVertex, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * stride, (const void*)offset);
	offset += elementsPerVertex * sizeof(GLfloat);
	layouts++;
}

MeshResource::~MeshResource(){
	glDeleteVertexArrays(1, &vertexArrayID);
}
