//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

MeshResource mr; // example mesh here for easy access in both run and open functions
TextureResource tr;
ShaderObject so;

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
		
		so.generateVertexShader("C:/Users/Kurowoomy/Documents/Universitetet/S0006E Programmering av realtidsgrafik/lab-env/engine/render/ShaderVertex.txt");
		so.generateFragmentShader("C:/Users/Kurowoomy/Documents/Universitetet/S0006E Programmering av realtidsgrafik/lab-env/engine/render/ShaderFragment.txt");
		so.generateProgram();

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
	float dx = 0.1, maxDistance = 2; // MeshResources lab animation
	Vec4 eye(0, 0, 3), target(0, 0, 0), up(0, 1, 0); // viewMatrix vectors
	float yRadians = 0; //viewMatrix values
	
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 100.0f);
	
	so.makeUniform("transformationMatrix"); //skapa location så man slipper göra det varje frame
	so.useProgram();
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

		so.uploadUniformMatrix4("transformationMatrix", mvp);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		this->window->SwapBuffers();
	}
	mr.vertexArrayUnbind();
	tr.unbindTexture();
	so.quitProgram();

	so.destroy();
	tr.destroyID();
	mr.destroyID();
}

} // namespace Example