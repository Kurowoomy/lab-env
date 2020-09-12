//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

#include "render/MeshResource.h"

// Läser av vertex buffern! O:
const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=0) out vec4 Color;\n"
""
"uniform mat4 rotationMatrix;\n"
"uniform vec4 translationVector;\n"
"uniform float scalar;\n"
""
"void main()\n"
"{\n"
"	gl_Position = rotationMatrix * vec4(pos, 1);\n"
"   gl_Position += translationVector;\n"
"   gl_Position = gl_Position * scalar;\n"
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

MeshResource mr; // example mesh

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
		mr.genVertexBuffer(buf, sizeof(buf), 3);
		mr.genIndexBuffer(ibuf, 6);

		mr.addArrayAttribute(3);

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
	mr.vertexArrayBind();
	glUseProgram(this->program);

	Matrix4 rotMat;
	Vec4 transVec;
	transVec.w = 0;
	float radians = 0, dx = 0.005, scalar = 0.5;
	int location = glGetUniformLocation(program, "rotationMatrix");
	int vecLocation = glGetUniformLocation(program, "translationVector");
	int scalarLocation = glGetUniformLocation(program, "scalar");
	
	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		this->window->Update();

		// do stuff
		// rotera fyrkanten 
		// TODO: få in allt i en enda transformMatrix så jag slipper tre olika uniforms.
		radians += 0.01;
		if (transVec.x <= -0.5) {
			dx = 0.006;
		}
		else if (transVec.x >= 0.5) {
			dx = -0.006;
		}
		transVec.x += dx;

		glUniformMatrix4fv(location, 1, GL_FALSE, &rotMat.rotationZ(radians)[0]);
		glUniform4fv(vecLocation, 1, &transVec.x);
		glUniform1f(scalarLocation, scalar);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		this->window->SwapBuffers();
	}
	mr.vertexArrayUnbind();
	glUseProgram(0);
}

} // namespace Example