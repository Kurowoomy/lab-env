#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

// my own includes
//#include "render/MeshResource.h"
//#include "render/ShaderObject.h"
//#include "render/TextureResource.h"
#include "render/GraphicsNode.h"

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open();
	/// run app
	void Run();

	// my own functions


private:

	GLuint program;
	GLuint vertexShader;
	GLuint pixelShader;
	GLuint triangle;
	Display::Window* window;

	// my own members
	bool hasPressedLeft = false, isRotating = false, hasPressedKey = false;
	GraphicsNode gn;
	float rotationSpeed = 0.01, moveSpeed = 0.1;
	float64 lastRadX = -1, lastRadY, lastMoveX, lastMoveY;
};
} // namespace Example