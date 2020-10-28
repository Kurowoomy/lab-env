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
					renderer.setTransform(Matrix4::translationMatrix(-moveSpeed, 0, 0) * renderer.getTransform());
					break;
				case 262: // right
					renderer.setTransform(Matrix4::translationMatrix(moveSpeed, 0, 0) * renderer.getTransform());
					break;
				case 265: // up
					renderer.setTransform(Matrix4::translationMatrix(0, moveSpeed, 0) * renderer.getTransform());
					break;
				case 264: // down
					renderer.setTransform(Matrix4::translationMatrix(0, -moveSpeed, 0) * renderer.getTransform());
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
						renderer.getTransform()[3], renderer.getTransform()[7], renderer.getTransform()[11]);
					Matrix4 invTranslation = Matrix4::translationMatrix(
						-renderer.getTransform()[3], -renderer.getTransform()[7], -renderer.getTransform()[11]);

					renderer.setTransform(translation * Matrix4::rotationY((x - lastMoveX) * rotationSpeed)
						* Matrix4::rotationX((y - lastMoveY) * rotationSpeed) * invTranslation * renderer.getTransform());

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
			Vec4 newPos, newNormal, newWorldPos;
			newPos = renderer.projectionMatrix * renderer.viewMatrix * renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			newWorldPos = renderer.model * Vec4(vertex.pos.x, vertex.pos.y, vertex.pos.z);
			renderer.worldPos = Vec3(newWorldPos.x, newWorldPos.y, newWorldPos.z);
			newNormal = renderer.model * Vec4(vertex.normal.x, vertex.normal.y, vertex.normal.z);
			vertex.normal = Vec3(newNormal.x, newNormal.y, newNormal.z);
			vertex.uv.x = vertex.uv.x * (renderer.texture.width); // size 512, uv positions are 0 - 511
			vertex.uv.y = vertex.uv.y * (renderer.texture.height);
			vertex.pos = Vec3(newPos.x, newPos.y, newPos.z);

			return newPos;
		});
		renderer.setPixelShader([this](Vec2 uvcoord, Vec3 Normal, unsigned char* textureColor)
		{
			Vec4 pixelColor; // object color, base
			Vec4 result; // final color after using pixelColor and lighting together

			// find position i in textureColor[i], use uvcoord.x and uvcoord.y
			//f�r varje y har det g�tt textureWidth i pixlar
			int i = renderer.texture.width * uvcoord.y * renderer.texture.comp + uvcoord.x * renderer.texture.comp;
			pixelColor.x = textureColor[i];
			pixelColor.y = textureColor[i + 1];
			pixelColor.z = textureColor[i + 2];
			pixelColor.w = textureColor[i + 3];
			//pixelColor = Vec4(100, 100, 100);

			// add lighting here
			// ambient
			float ambientStrength = 0.4;
			Vec4 ambient = Vec4(ambientStrength * pl.color.x, ambientStrength * pl.color.y, ambientStrength * pl.color.z);

			// diffuse
			Vec3 normal = Normal.normalize();
			Vec3 lightDir = (pl.position - renderer.worldPos).normalize();
			float dotDiffuse = renderer.max(normal * lightDir, 0.0f);
			Vec3 diffuse = pl.color * dotDiffuse;

			// specular
			Vec3 viewDir = (renderer.cameraPos - renderer.worldPos).normalize();
			Vec3 halfwayDir = (lightDir + viewDir).normalize();
			float dotSpecular = renderer.pow(renderer.max(normal * halfwayDir, 0.0f), 64);
			Vec3 specular = pl.color * pl.intensity * dotSpecular;


			result = Vec4(	(specular.x + diffuse.x + ambient.x) * pixelColor.x, 
							(specular.y + diffuse.y + ambient.y) * pixelColor.y,
							(specular.z + diffuse.z + ambient.z) * pixelColor.z);
			return result;
		});

		int width, height;
		this->window->GetSize(width, height);
		
		
		renderer.loadTextureFile("projects/Rasterizer/aqua.png");

		renderer.cameraPos = Vec3(0, 0, 10);
		renderer.viewMatrix = Matrix4::viewMatrix(Vec4(renderer.cameraPos.x, renderer.cameraPos.y, renderer.cameraPos.z), Vec4(0, 0, 0), Vec4(0, 1, 0));
		renderer.projectionMatrix = Matrix4::perspectiveMatrix(90, (float)width / (float)height, renderer.near, renderer.far);

		renderer.framebuffer.width = width;
		renderer.framebuffer.height = height;
		
		
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
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		this->window->Update();

		// do stuff
		renderer.draw(renderer.addVertexIndexBuffer("engine/render/dragon.obj"));

		glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer.framebufferID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBlitFramebuffer(0, 0, renderer.framebuffer.width, renderer.framebuffer.height, 0, 0, renderer.framebuffer.width, renderer.framebuffer.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);


		this->window->SwapBuffers();
	}
	
}

} // namespace Example