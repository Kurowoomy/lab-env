//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>

GraphicsNode gn;

//------ Mouse movement ---------------------------------------
Display::Window* testWindow;

static void test(int b, int c) {
	printf("%i", b);
	printf(" ");
	printf("%i", c);
	gn.setTransform(Matrix4::translationMatrix(0, 0, 0)
		* Matrix4::rotationY(c*0.01) * Matrix4::rotationX(b*0.01) * Matrix4::scaleMatrix(1));
}
static void stopMouseMovement(int a, int b) {

}

static void test2(int a, int b, int c) {
	if (a == 0) {
		testWindow->SetMouseMoveFunction(test);
	}
	if (a == 1) {
		testWindow->SetMouseMoveFunction(stopMouseMovement);
	}
	if (a == 2) {
		printf("2 pressed");
	}
}
//-------------------------------------------------------------

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
	// TODO: kolla närmare på detta och fortsätt eventuellt i samma stil som ovan
	window->SetKeyPressFunction([this](int32, int32, int32, int32)
	{
		this->window->Close();
	});

	if (this->window->Open())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		gn.setMesh(MeshResource::Shape::CUBE, { 3, 2 }, 1);		
		gn.setShader("C:/Users/emmeli-8-local/Documents/S0006E Programmering av realtidsgrafik/lab-env/engine/render/ShaderVertex.txt", 
			"C:/Users/emmeli-8-local/Documents/S0006E Programmering av realtidsgrafik/lab-env/engine/render/ShaderFragment.txt");
		gn.setTexture("flower_texture.png");

		testWindow = window;
		
		window->SetMousePressFunction(test2);


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
	Vec4 transVec; // modelMatrix vector
	float modelRadiansY = 0, scalar = 1; // modelMatrix values
	float dx = 0.1, maxDistance = 2; // MeshResources lab animation
	Vec4 eye(0, 0, 3), target(0, 0, 0), up(0, 1, 0); // viewMatrix vectors
	float yRadians = 0; //viewMatrix values
	int width, height;
	this->window->GetSize(width, height);
	projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 100.0f);
	
	gn.getShader().makeUniform("transformationMatrix"); //skapa location så man slipper göra det varje frame

	while (this->window->IsOpen())
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff
		//------- Rotation and translation ------------------------------

		// TODO: read mouse movement and use it to change rotation
		

		/*gn.setTransform(Matrix4::translationMatrix(transVec.x, transVec.y, transVec.z)
			* Matrix4::rotationY(modelRadiansY) * Matrix4::scaleMatrix(scalar));*/
		//---------------------------------------------------------------
		
		//------- Rotate camera while looking at cube position ----------
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