//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

// mina inkluderingar
#include "render/MeshResource.h"
#include "core/Math_Library.h"

// Läser av vertex buffern! O:
const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=0) out vec4 Color;\n"
""
"uniform mat4 transformationMatrix;\n"
""
"void main()\n"
"{\n"
"   gl_Position = transformationMatrix * vec4(pos, 1);\n"
"	Color = vec4(abs(pos.x+0.5), abs(pos.y+0.5), abs(pos.z), 1);\n"
"}\n";

// Målar efter instruktioner (i detta fall från output från vs!) O:
const GLchar* ps =
"#version 430\n"
"in vec4 color;\n"
"out vec4 Color;\n"
"void main()\n"
"{\n"
"	Color = color;\n"
"}\n";

MeshResource mr; // example mesh here for easy access in both run and open functions

using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int32, int32, int32, int32)
	{
		this->window->Close();
	});

	GLfloat buf[] =
	{
		-0.5f,	-0.5f,	-1,			// pos 0
		-0.5f,	0.5f,	-1,			// pos 1
		0.5f,	0.5f,	-1,			// pos 2
		0.5f,	-0.5f,	-1,
		/*-0.5f,	-0.5f,	-1,
		0.5f,	-0.5f,	-1,
		0.5f,	0.5f,	-1,
		-0.5f,	0.5f,	-1*/
	};
	GLuint ibuf[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		

		// setup vertex shader
		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = static_cast<GLint>(std::strlen(vs));
		glShaderSource(this->vertexShader, 1, &vs, &length);
		glCompileShader(this->vertexShader);

		// get error log
		GLint shaderLogSize;
		glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// setup pixel shader
		this->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
		length = static_cast<GLint>(std::strlen(ps));
		glShaderSource(this->pixelShader, 1, &ps, &length);
		glCompileShader(this->pixelShader);

		// get error log
		shaderLogSize;
		glGetShaderiv(this->pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->pixelShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}

		// create a program object
		this->program = glCreateProgram();
		glAttachShader(this->program, this->vertexShader);
		glAttachShader(this->program, this->pixelShader);
		glLinkProgram(this->program);
		glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
			printf("[PROGRAM LINK ERROR]: %s", buf);
			delete[] buf;
		}

		// setup vba
		mr.genVertexArray();
		mr.generateCube(1);

		mr.addArrayAttribute(3, 5); // x, y, z for each vertex

		mr.vertexArrayUnbind();
		mr.vertexUnbind();
		mr.indexUnbind();

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	// animation setup
	Matrix4 transformationMatrix;
	Vec4 transVec;
	float radians = 0, dx = 0.006, scalar = 0.5, maxDistance = 0.75;
	int transformationLocation = glGetUniformLocation(program, "transformationMatrix");
	
	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		this->window->Update();

		// do stuff
		// rotate 0.02 radians per frame
		radians += 0.02;
		// translation animation left to right. Change direction if maxDistance reached.
		if (transVec.x <= -maxDistance || transVec.x >= maxDistance) {
			dx = -dx;
		}
		transVec.x += dx;
		
		// send updated data to shader
		transformationMatrix = Matrix4::translationMatrix(transVec.x, transVec.y, transVec.z)
			* Matrix4::rotationZ(radians) * Matrix4::scaleMatrix(scalar);
		
		glUseProgram(this->program);
		
		glUniformMatrix4fv(transformationLocation, 1, GL_TRUE, &transformationMatrix[0]);

		mr.vertexArrayBind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		mr.vertexArrayUnbind();
		glUseProgram(0);

		this->window->SwapBuffers();
	}
}

} // namespace Example