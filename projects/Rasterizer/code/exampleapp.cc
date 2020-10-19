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
		int width, height;
		this->window->GetSize(width, height);
		renderer.setFramebuffer(width, height);

		renderer.loadTextureFile("projects/Rasterizer/flower_texture.png");

		viewMatrix = Matrix4::viewMatrix(Vec4(0, 0, 10), Vec4(0, 0, 0), Vec4(0, 1, 0)); 
		projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 3000.0f);

		renderer.setVertexShader([this](Vertex& vertex) 
		{
			Vec4 newPos, newNormal, newWorldPos, newTexCoords;
			newPos = projectionMatrix * viewMatrix * renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			newWorldPos = renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			renderer.worldPos = Vec3(newWorldPos.x, newWorldPos.y, newWorldPos.z);
			newNormal = renderer.model * Vec4(vertex.normal.x, vertex.normal.y, vertex.normal.z);
			vertex.normal = Vec3(newNormal.x, newNormal.y, newNormal.z);
			newTexCoords = renderer.model * Vec4(vertex.uv.x, vertex.uv.y, 0);
			vertex.uv = Vec2(newTexCoords.x, newTexCoords.y);
			vertex.pos = Vec3(newPos.x, newPos.y, newPos.z);

			return newPos;
		});
		renderer.setPixelShader([](Vec2 uvcoord, Vec3 normal, unsigned char* textureColor)
		{
			Vec4 pixelColor;



			return pixelColor;
		});
		renderer.draw(renderer.addVertexIndexBuffer("engine/render/dragon.obj"));
		
		// create a graphics node object (screen to put texture on)
		gn.setMesh("engine/render/square.obj");
		gn.setShader("engine/render/vs.txt", 
			"engine/render/fs.txt");

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
	while (this->window->IsOpen())
	{
		// set clear color to gray
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff



		this->window->SwapBuffers();
	}
	gn.destroyAll();
}

} // namespace Example