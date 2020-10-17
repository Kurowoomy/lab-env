#include "config.h"
#include "Renderer.h"



void* Renderer::addVertexIndexBuffer(const char* objPath)
{
	if (buffers.indexBuffer.empty()) {
		// read objFile
		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<Vec3> tempVertices, tempNormals;
		std::vector<Vec2> tempUvs;
		FILE* objFile = fopen(objPath, "r");
		if (objFile == NULL) {
			printf("Impossible to open the obj file !\n");
		}
		while (true) {
			char firstWord[128] = { 0 };
			int readBytes = fscanf(objFile, "%s", firstWord);
			if (readBytes == EOF) {
				break;
			}
			if (strcmp(firstWord, "v") == 0) {
				Vec3 vertex;
				if (!fscanf(objFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z)) {
					printf("Can't read v line\n");
				}
				tempVertices.push_back(vertex);
			}
			else if (strcmp(firstWord, "vt") == 0) {
				Vec2 uv;
				if (!fscanf(objFile, "%f %f\n", &uv.x, &uv.y)) {
					printf("Can't read vt line\n");
				}
				tempUvs.push_back(uv);
			}
			else if (strcmp(firstWord, "vn") == 0) {
				Vec3 normal;
				if (!fscanf(objFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z)) {
					printf("Can't read vn line\n");
				}
				tempNormals.push_back(normal);
			}
			else if (strcmp(firstWord, "f") == 0) {
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(objFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}

		// make mesh
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {
			Vec3 vertex = tempVertices[vertexIndices[i] - 1];
			mesh.vertices.push_back(vertex);
		}
		for (unsigned int i = 0; i < uvIndices.size(); i++) {
			Vec2 uv = tempUvs[uvIndices[i] - 1];
			uv.y = 1 - uv.y;
			mesh.uvs.push_back(uv);
		}
		for (unsigned int i = 0; i < normalIndices.size(); i++) {
			Vec3 normal = tempNormals[normalIndices[i] - 1];
			mesh.normals.push_back(normal);
		}
		for (int i = 0; i < vertexIndices.size(); i++) {
			// write all values into the buffers in this renderer class
			buffers.indexBuffer.push_back(i);
			Vertex v = Vertex(mesh.vertices[i], mesh.normals[i], mesh.uvs[i]);
			buffers.vertexBuffer.push_back(v);
		}
	}

	return &buffers;
}

void Renderer::setFramebuffer(unsigned int width, unsigned int height) 
{
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);

	framebuffer.width = width;
	framebuffer.height = height;
	framebuffer.colorBuffer.resize(height);
	for (int i = 0; i < framebuffer.colorBuffer.size(); i++) {
		framebuffer.colorBuffer[i].resize(width);
	}
	
	glGenTextures(1, &colorID);
	glBindTexture(GL_TEXTURE_2D, colorID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("D: Framebuffer incomplete.");
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
unsigned int Renderer::getFramebufferWidth() {
	return framebuffer.width;
}
unsigned int Renderer::getFramebufferHeight() {
	return framebuffer.height;
}
void Renderer::bindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
}
void Renderer::unbindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2) 
{
	// omvandling av vertiserna så de befinner sig i rätt position i förhållande till worldspace
	Vec4 newPos0 = vertexShader(v0);
	Vec4 newPos1 = vertexShader(v1);
	Vec4 newPos2 = vertexShader(v2);
	// -----------------------------------------------------------------------------------------

	// omvandla till rasteriseringskoordinater -------------------------------------------------
	// ska vara från 0 till width och 0 till height.
	v0.pos = convertToRasterSpace(newPos0);
	v1.pos = convertToRasterSpace(newPos1);
	v2.pos = convertToRasterSpace(newPos2);

	// -----------------------------------------------------------------------------------------

	// Måste först hitta alla pixlar man behöver jobba med för denna triangel-------------------
	std::vector<Vec2> line0 = createLine(v0.pos.x, v1.pos.x, v0.pos.y, v1.pos.y);
	std::vector<Vec2> line1 = createLine(v1.pos.x, v2.pos.x, v1.pos.y, v2.pos.y);
	std::vector<Vec2> line2 = createLine(v2.pos.x, v0.pos.x, v2.pos.y, v0.pos.y);

	// sortera y-värdena så [0].y är lägst och [size()].y är högst
	if (line0[0].y > line0[line0.size()].y) {
		std::reverse(line0.begin(), line0.end());
	}
	if (line1[0].y > line1[line1.size()].y) {
		std::reverse(line1.begin(), line1.end());
	}
	if (line2[0].y > line2[line2.size()].y) {
		std::reverse(line2.begin(), line2.end());
	}

	// TODO: (interpolera värden via barycentric coordinates? o:)
	// (man beräknar barycentric coordinates för varje pixel!)
	pixels.clear();
	normals.clear();
	uvCoords.clear();
	// TODO: fill std::vector<std::vector<int>> with pixels (fill triangle)

	
	

	// -----------------------------------------------------------------------------------------

	// pixel shader ----------------------------------------------------------------------------
	// TODO: använd pixelShader-metoden här. och textureColor. [i], [i+1], [i+2], [i+3]  = r, g, b, a.
	// textureColor = objectColor. 

	// -----------------------------------------------------------------------------------------
}

void Renderer::draw(void* handle) 
{
	Buffers* buffers = (Buffers*)handle;
	
	for (int i = 0; i < buffers->indexBuffer.size(); i+=3) {
		rasterizeTriangle(buffers->vertexBuffer[i], buffers->vertexBuffer[i + 1], buffers->vertexBuffer[i + 2]);
	}
}

void Renderer::setVertexShader(const std::function<Vec4(Vertex&)> vertexShader) {
	this->vertexShader = vertexShader;
}
void Renderer::setFragmentShader(void(*fragmentShader)(Vec3)) {
	this->fragmentShader = fragmentShader;
}

void Renderer::loadTextureFile(const char* fileName) {
	textureColor = stbi_load(fileName, &textureWidth, &textureHeight, &channels, 0);
}

std::vector<Vec2> Renderer::createLine(int x0, int x1, int y0, int y1) 
{
	std::vector<Vec2> newLine;

	int dx, dy, x, y, xEnd, yEnd, yFault, xFault;
	dx = x1 - x0;
	dy = y1 - y0;
	yFault = 2 * abs(dy) - abs(dx);
	xFault = 2 * abs(dx) - abs(dy);

	if (abs(dy) <= abs(dx)) // increment x for each pixel
	{
		if (dx >= 0) 
		{
			// gå från x0 till x1
			x = x0;
			y = y0;
			xEnd = x1;
		}
		else 
		{
			// gå från x1 till x0
			x = x1;
			y = y1;
			xEnd = x0;
		}

		// go to the right, x++
		newLine.push_back(Vec2(x, y));
		for (int i = 0; x < xEnd; i++) 
		{
			x++;

			if (yFault < 0) { // y is still close to the line
				yFault += 2 * abs(dy);
			}
			else // y is too far from the line
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
					y++; // slope goes down
				}
				else {
					y--; // slope goes up
				}
				yFault += 2 * (abs(dy) - abs(dx));
			}

			newLine.push_back(Vec2(x, y));
		}
	}
	else // increment y for each pixel
	{ 
		if (dy >= 0) 
		{
			// gå från y0 till y1
			x = x0;
			y = y0;
			yEnd = y1;
		}
		else 
		{
			// gå från y1 till y0
			x = x1;
			y = y1;
			yEnd = y0;
		}

		// go down, y++
		newLine.push_back(Vec2(x, y));
		for (int i = 0; y < yEnd; i++) {
			y++;

			if (xFault < 0) {
				xFault += 2 * abs(dx);
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
					x++;
				}
				else {
					x--;
				}
				xFault += 2 * (abs(dx) - abs(dy));
			}

			newLine.push_back(Vec2(x, y));
		}
	}

	return newLine;
}

Vec3 Renderer::convertToRasterSpace(Vec4& v) 
{
	Vec3 pos;
	if (v.w != 0) {
		pos = Vec3(v.x / v.w, v.y / v.w, v.z / v.w);
	}
	else {
		printf("w is 0 :( \n");
	}

	Vec2 windowSpacePos = Vec2((pos.x + 1) / 2 * framebuffer.width, (1 - pos.y) / 2 * framebuffer.height);
	pos.x = windowSpacePos.x;
	pos.y = windowSpacePos.y;

	return pos;
}

void Renderer::fillTriangle(std::vector<Vec2> line0, std::vector<Vec2> line1, std::vector<Vec2> line2)
{
	// check which lines share the top-most vertex (lowest y)
	// use min(a, b) math function?
}


