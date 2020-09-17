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
"layout(location=0) out vec4 Color;\n"
//"out vec2 texCoord;\n"
""
"uniform mat4 transformationMatrix;\n"
""
"void main()\n"
"{\n"
"   gl_Position = transformationMatrix * vec4(pos, 1);\n"
//"   texCoord = uv;\n"
"	Color = vec4(abs(pos.x+0.5), abs(pos.y+0.5), abs(pos.z), 1);\n"
"}\n";

// Målar efter instruktioner (i detta fall från output från vs!) O:
const GLchar* ps =
"#version 430\n"
"in vec4 color;\n"
//"in vec2 texCoord;\n"
"out vec4 Color;\n"
"uniform sampler2D texColor;\n"
"void main()\n"
"{\n"
//"	Color = texture(texColor, texCoord);\n"
"   Color = color;"
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

	GLfloat buf[] =
	{
		-0.5f,	-0.5f,	-1,			// pos 0
		0, 0,						// uv coordinates pos 0
		-0.5f,	0.5f,	-1,			// pos 1
		0, 1,
		0.5f,	0.5f,	-1,			// pos 2
		1, 1,
		0.5f,	-0.5f,	-1,
		1, 0
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
	Matrix4 inverseModel;
	Vec4 transVec;
	Vec4 eye(0, 0, 9), target, up(0, 1, 0);
	float radians = 0, dx = 0.006, scalar = 0.5, maxDistance = 0.75; //TODO: scalar och texturestorlek krockar
	int transformationLocation = glGetUniformLocation(program, "transformationMatrix");
	
	glUseProgram(this->program);
	tr.bindTexture();
	mr.vertexArrayBind();
	
	//viewMatrix = Matrix4::viewMatrix(;
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 100.0f);
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
		// TODO: mvp, modelmatrix redan klar (transformationMatrix)
		
		modelMatrix = Matrix4::translationMatrix(transVec.x, transVec.y, transVec.z)
			* Matrix4::rotationZ(radians) * Matrix4::scaleMatrix(scalar);
		// compute eye vector from inverse model matrix column 4 (x, y, z)
		Matrix4::inverse(modelMatrix, inverseModel);
		target.x = modelMatrix[0];
		target.y = 0;
		target.z = modelMatrix[10];
		viewMatrix = Matrix4::viewMatrix(eye, target, up);

		//mvp = projectionMatrix * viewMatrix * modelMatrix; //kan ha med projection matrix att göra..??
		mvp = modelMatrix;


		glUniformMatrix4fv(transformationLocation, 1, GL_TRUE, &mvp[0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		this->window->SwapBuffers();
	}
	mr.vertexArrayUnbind();
	tr.unbindTexture();
	glUseProgram(0);
}

} // namespace Example