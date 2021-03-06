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
				case 263: // left
					gn.setTransform(Matrix4::translationMatrix(-moveSpeed, 0, 0) * gn.getTransform());
					break;
				case 262: // right
					gn.setTransform(Matrix4::translationMatrix(moveSpeed, 0, 0) * gn.getTransform());
					break;
				case 265: // up
					gn.setTransform(Matrix4::translationMatrix(0, moveSpeed, 0) * gn.getTransform());
					break;
				case 264: // down
					gn.setTransform(Matrix4::translationMatrix(0, -moveSpeed, 0) * gn.getTransform());
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

					gn.setTransform(translation * Matrix4::rotationY((x - lastMoveX) * rotationSpeed)
						* Matrix4::rotationX((y - lastMoveY) * rotationSpeed) * invTranslation * gn.getTransform());

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
		gn.setMesh("engine/render/dragon.obj");
		gn.setShader("engine/render/ShaderVertex.txt", 
			"engine/render/ShaderFragment.txt");
		gn.setTexture("projects/ObjectFiles/aqua.png");
		
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
	float yRadians = 0, lightradians = 0; //viewMatrix values
	Vec4 eye(0, 0, 10), target(0, 0, 0), up(0, 1, 0); // viewMatrix vectors
	Vec3 cameraPos(eye.x, eye.y, eye.z);

	// create projection matrix
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 3000.0f);
	
	// create shader location once before loop
	gn.getShader().makeUniform("model");
	gn.getShader().makeUniform("view");
	gn.getShader().makeUniform("projection");
	gn.getShader().makeUniform("lightColor");
	gn.getShader().makeUniform("lightPos");
	gn.getShader().makeUniform("lightIntensity");
	gn.getShader().makeUniform("cameraPos");

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff
		//------- Automatic rotation and translation ------------------------------
		/*gn.setTransform(Matrix4::translationMatrix(0, 0, 0) * Matrix4::rotationY(modelRadians * rotationSpeed)
			* Matrix4::rotationX(modelRadians * rotationSpeed) * Matrix4::scaleMatrix(scalar) * gn.getTransform());*/
		/*lightradians = 0.0001;
		pl.transform = Matrix4::translationMatrix(0, 0, 0) * Matrix4::rotationY(lightradians);
		pl.position = Vec3(	(pl.transform * Vec4(pl.position.x, pl.position.y, pl.position.z)).x, 
							(pl.transform * Vec4(pl.position.x, pl.position.y, pl.position.z)).y, 
							(pl.transform * Vec4(pl.position.x, pl.position.y, pl.position.z)).z);*/
		//---------------------------------------------------------------
		
		//------- Automatic camera rotation while looking at cube position ----------
		yRadians += 0.02;
		viewMatrix = Matrix4::viewMatrix(eye, target, up) * Matrix4::rotationY(0); // rotate first, then center camera
		//---------------------------------------------------------------

		gn.getShader().useProgram();
		gn.getShader().uploadUniformMatrix4("model", gn.getTransform());
		gn.getShader().uploadUniformMatrix4("view", viewMatrix);
		gn.getShader().uploadUniformMatrix4("projection", projectionMatrix);
		gn.getShader().uploadUniformVector3("lightColor", pl.color);
		gn.getShader().uploadUniformVector3("lightPos", pl.position);
		gn.getShader().uploadUniformFloat("lightIntensity", pl.intensity);
		gn.getShader().uploadUniformVector3("cameraPos", cameraPos);
		gn.draw();

		this->window->SwapBuffers();
	}
	gn.destroyAll();
}

} // namespace Example