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
#include "render/TextureResource.h"

// Läser av vertex buffern! O:
const GLchar* vs =
"#version 430\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec2 uv;\n"
"out vec2 texCoord;\n"
""
"uniform mat4 transformationMatrix;\n"
""
"void main()\n"
"{\n"
"   gl_Position = transformationMatrix * vec4(pos, 1);\n"
"   texCoord = uv;\n"
"}\n";

// Målar efter instruktioner (i detta fall från output från vs!) O:
const GLchar* ps =
"#version 430\n"
"in vec2 texCoord;\n"
"out vec4 Color;\n"
"uniform sampler2D texColor;\n"
"void main()\n"
"{\n"
"	Color = vec4(texture(texColor, texCoord).rgb, 1.0f);\n"
"}\n";

MeshResource mr; // example mesh here for easy access in both run and open functions
TextureResource tr;

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

		// setup vao
		mr.genVertexArray();
		mr.generateCube(1);

		mr.addArrayAttribute(3); // x, y, z for each vertex
		mr.addArrayAttribute(2);

		mr.vertexArrayUnbind();
		mr.vertexUnbind();
		mr.indexUnbind();

		tr.loadFromFile("flower_texture.png");

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
	Matrix4 modelMatrix;
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	Matrix4 mvp;
	Vec4 transVec; // modelMatrix vector
	float modelRadiansY = 0, scalar = 1; // modelMatrix values
	float dx = 0.05, maxDistance = 2; // MeshResources lab animation
	Vec4 eye(0, 0, 3), target(0, 0, 0), up(0, 1, 0); // viewMatrix vectors
	float yRadians = 0; //viewMatrix values
	
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 100.0f);
	
	int transformationLocation = glGetUniformLocation(program, "transformationMatrix");
	glUseProgram(this->program);
	tr.bindTexture();
	mr.vertexArrayBind();

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff

		//------- Rotation and translation from left to right -----------
		// rotate 0.02 radians per frame
		//modelRadiansY -= 0.2;
		//translation animation left to right. Change direction if maxDistance reached.
		/*if (transVec.x <= -maxDistance || transVec.x >= maxDistance) {
			dx = -dx;
		}
		transVec.x += dx;*/
		//---------------------------------------------------------------
		
		//------- Rotate camera while looking at cube position ----------
		yRadians += 0.02;
		//---------------------------------------------------------------

		modelMatrix = Matrix4::translationMatrix(transVec.x, transVec.y, transVec.z)
			* Matrix4::rotationY(modelRadiansY) * Matrix4::scaleMatrix(scalar);
		viewMatrix = Matrix4::viewMatrix(eye, target, up) * Matrix4::rotationY(yRadians); // rotate first, then center camera
		mvp = projectionMatrix * viewMatrix * modelMatrix;

		glUniformMatrix4fv(transformationLocation, 1, GL_TRUE, &mvp[0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		this->window->SwapBuffers();
	}
	mr.vertexArrayUnbind();
	tr.unbindTexture();
	glUseProgram(0);

	glDeleteProgram(program);
	tr.destroyID();
	mr.destroyID();
}

} // namespace Example