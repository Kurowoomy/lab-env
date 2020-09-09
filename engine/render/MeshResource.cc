#include "config.h"
#include "MeshResource.h"

MeshResource::MeshResource() {
	vertexID = 0;
	indexID = 0;
	count = 0;
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

void MeshResource::genVertexBuffer(const GLfloat* buf, GLuint size) {
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
void MeshResource::genVertexArray() {
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
}

//void MeshResource::setVertexAttributes(GLuint layout, )

MeshResource::~MeshResource(){
	glDeleteBuffers(1, &vertexID);
	glDeleteBuffers(1, &indexID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

//VertexBuffer::VertexBuffer() //members initialized unlike in exampleapp
//{
//	rendererID = 0;
//}
// Binds buffer
//void VertexBuffer::genVertexBuffer(const GLfloat* buf, const GLuint size)
//{
//	glGenBuffers(1, &this->rendererID);
//	glBindBuffer(GL_ARRAY_BUFFER, this->rendererID);
//	glBufferData(GL_ARRAY_BUFFER, size, buf, GL_STATIC_DRAW);
//}
//void VertexBuffer::Bind() const
//{
//	glBindBuffer(GL_ARRAY_BUFFER, rendererID);
//}
//void VertexBuffer::Unbind() const
//{
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//VertexBuffer::~VertexBuffer()
//{
//}
//
//IndexBuffer::IndexBuffer() // members initialized unlike in exampleapp
//{
//	rendererID = 0;
//	count = 0;
//}
//void IndexBuffer::genIndexBuffer(const GLuint* data, GLuint count)
//{
//	this->count = count;
//	glGenBuffers(1, &rendererID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
//}
//void IndexBuffer::Bind() const
//{
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
//}
//void IndexBuffer::Unbind() const
//{
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}
//IndexBuffer::~IndexBuffer()
//{
//}

