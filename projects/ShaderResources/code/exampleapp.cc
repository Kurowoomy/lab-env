//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"

#include <cstring>

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
	window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mods)
	{
			if (action == 1 || action == 2) { // 1 = pressed, 2 = holding, 0 = released 
				switch (key) {
				case GLFW_KEY_LEFT: // left
					gn.setTransform(Matrix4::translationMatrix(moveSpeed, 0, 0) * gn.getTransform());
					break;
				case GLFW_KEY_RIGHT: // right
					gn.setTransform(Matrix4::translationMatrix(-moveSpeed, 0, 0) * gn.getTransform());
					break;
				case GLFW_KEY_UP: // up
					gn.setTransform(Matrix4::translationMatrix(0, -moveSpeed, 0) * gn.getTransform());
					break;
				case GLFW_KEY_DOWN: // down
					gn.setTransform(Matrix4::translationMatrix(0, moveSpeed, 0) * gn.getTransform());
					break;
				default:
					this->window->Close();
					break;
				}
			}
	});
	window->SetMousePressFunction([this](int32 button, int32 action, int32 mods)
	{
		if (button == 0) {
			if (!hasPressedLeft) {
				hasPressedLeft = true;
				window->SetMouseMoveFunction([this](float64 x, float64 y)
				{
					if (!isRotating) {
						isRotating = true;
						lastRadX = x;
						lastRadY = y;
						lastMoveX = x;
						lastMoveY = y;
					}

					Matrix4 translation = Matrix4::translationMatrix(
						gn.getTransform()[3], gn.getTransform()[7], gn.getTransform()[11]);
					Matrix4 invTranslation = Matrix4::translationMatrix(
						-gn.getTransform()[3], -gn.getTransform()[7], -gn.getTransform()[11]);
					
					gn.setTransform(translation * Matrix4::rotationY(-(x - lastMoveX) * rotationSpeed)
						* Matrix4::rotationX(-(y - lastMoveY) * rotationSpeed) * invTranslation * gn.getTransform());
					
					lastRadX += x - lastMoveX;
					lastRadY += y - lastMoveY;
					lastMoveX = x;
					lastMoveY = y;
				});
			}
			else {
				hasPressedLeft = false;
				isRotating = false;
				window->SetMouseMoveFunction([this](float64 x, float64 y)
				{
					// do nothing
				});
			}
				
		}
	});

	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// create a graphics node object
		gn.setMesh(MeshResource::Shape::CUBE, { 3, 2 }, 1);	
		gn.setShader("engine/render/ShaderVertex.txt", 
			"engine/render/ShaderFragment.txt");
		gn.setTexture("projects/ShaderResources/flower_texture.png");
		
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
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
	Matrix4 mvp;
	float modelRadians = 0, scalar = 1; // modelMatrix values
	float yRadians = 0; //viewMatrix values
	Vec4 eye(0, 0, 3), target(0, 0, 0), up(0, 1, 0); // viewMatrix vectors

	// create projection matrix
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 100.0f);
	
	// create shader location once before loop
	gn.getShader().makeUniform("transformationMatrix");

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff
		//------- Automatic rotation and translation ------------------------------
		/*gn.setTransform(Matrix4::translationMatrix(0, 0, 0) * Matrix4::rotationY(modelRadians * rotationSpeed) *
			 Matrix4::scaleMatrix(1) * gn.getTransform());*/
		//---------------------------------------------------------------
		
		//------- Automatic camera rotation while looking at cube position ----------
		yRadians += 0.02;
		viewMatrix = Matrix4::viewMatrix(eye, target, up) * Matrix4::rotationY(0); // rotate first, then center camera
		//---------------------------------------------------------------

		gn.getShader().useProgram();
		gn.getShader().uploadUniformMatrix4("transformationMatrix", projectionMatrix * viewMatrix * gn.getTransform());
		gn.draw();

		this->window->SwapBuffers();
	}
	gn.destroyAll();
}

} // namespace Example