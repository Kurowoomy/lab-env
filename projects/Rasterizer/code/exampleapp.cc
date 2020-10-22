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
		renderer.setVertexShader([this](Vertex& vertex) 
		{
			Vec4 newPos, newNormal, newWorldPos, newTexCoords;
			newPos = projectionMatrix * viewMatrix * renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			newWorldPos = renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			renderer.worldPos = Vec3(newWorldPos.x, newWorldPos.y, newWorldPos.z);
			newNormal = renderer.model * Vec4(vertex.normal.x, vertex.normal.y, vertex.normal.z);
			vertex.normal = Vec3(newNormal.x, newNormal.y, newNormal.z);
			vertex.uv.x = vertex.uv.x * (renderer.texture.width); // size 512, uv positions are 0 - 511
			vertex.uv.y = vertex.uv.y * (renderer.texture.height);
			vertex.pos = Vec3(newPos.x, newPos.y, newPos.z);

			return newPos;
		});
		renderer.setPixelShader([this](Vec2 uvcoord, Vec3 normal, unsigned char* textureColor)
		{
			Vec4 pixelColor;

			// find position i in textureColor[i], use uvcoord.x and uvcoord.y
			//för varje y har det gått textureWidth i pixlar
			int i = renderer.texture.width * uvcoord.y * renderer.texture.comp + uvcoord.x * renderer.texture.comp;
			pixelColor.x = textureColor[i];
			pixelColor.y = textureColor[i + 1];
			pixelColor.z = textureColor[i + 2];
			pixelColor.w = textureColor[i + 3];

			return pixelColor; // detta borde vara tillräckligt för att se kuben
		});

		int width, height;
		this->window->GetSize(width, height);
		
		
		renderer.loadTextureFile("projects/Rasterizer/minecraft_dirt3.png");


		viewMatrix = Matrix4::viewMatrix(Vec4(0, 0, 10), Vec4(0, 0, 0), Vec4(0, 1, 0)); 
		projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, 0.1f, 3000.0f);

		renderer.framebuffer.width = width;
		renderer.framebuffer.height = height;
		
		renderer.draw(renderer.addVertexIndexBuffer("engine/render/cube.obj"));
		
		renderer.setFramebuffer(width, height);
		
		// create a graphics node object (screen to put texture on)
		/*gn.setMesh("engine/render/square.obj");
		gn.setShader("engine/render/vs.txt", "engine/render/fs.txt");
		gn.setTexture(renderer.texture);*/

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

		
		

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//renderer.draw(renderer.addVertexIndexBuffer("engine/render/cube.obj"));

		this->window->Update();

		// do stuff
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.framebufferID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, renderer.framebuffer.width, renderer.framebuffer.height, 0, 0, renderer.framebuffer.width, renderer.framebuffer.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);


		this->window->SwapBuffers();
	}
	
}

} // namespace Example