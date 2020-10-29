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
#include "render/GraphicsNode.h"
#include "render/PointLight.h"

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

	Display::Window* window;

	// my own members
	GraphicsNode gn;
	PointLight pl;
	// for moving, rotating with input
	bool hasPressedLeft = false, isRotating = false, hasPressedKey = false;
	float rotationSpeed = 0.01, moveSpeed = 1;
	float64 lastRadX = -1, lastRadY, lastMoveX, lastMoveY;
};
} // namespace Example