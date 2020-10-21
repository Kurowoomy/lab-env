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
	
	//glEnable(GL_TEXTURE_2D);
	texture.genTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, &framebuffer.colorBuffer[0]);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	texture.unbindTexture();

	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);
	/*glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer()*/

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
	if (line0[0].y > line0[line0.size() - 1].y) {
		std::reverse(line0.begin(), line0.end());
	}
	if (line1[0].y > line1[line1.size() - 1].y) {
		std::reverse(line1.begin(), line1.end());
	}
	if (line2[0].y > line2[line2.size() - 1].y) {
		std::reverse(line2.begin(), line2.end());
	}

	// fill triangle and interpolate vertex attributes
	// use these three only for this triangle and pixel shader
	pixels.clear();
	normals.clear();
	uvCoords.clear();
	//int i = pixels.size();
	fillTriangle(line0, line1, line2); 
	//pixels vector is ready to use for interpolation!
	for (std::pair<int, int> pixel : pixels) {
		// make all pos into int because they're coordinates
		v0.pos.x = (int)v0.pos.x;
		v0.pos.y = (int)v0.pos.y;
		v1.pos.x = (int)v1.pos.x;
		v1.pos.y = (int)v1.pos.y;
		v2.pos.x = (int)v2.pos.x;
		v2.pos.y = (int)v2.pos.y;

		interpolate(pixel.first, pixel.second, v0, v1, v2);
		// probably need to sort v0, v1, v2 by y value
		/*if (min(v0.pos.y, min(v1.pos.y, v2.pos.y)) == v0.pos.y) {
			if (v1.pos.y <= v2.pos.y) {
				interpolate(pixel.first, pixel.second, v0, v1, v2);
			}
			else {
				interpolate(pixel.first, pixel.second, v0, v2, v1);
			}
		}
		else if (min(v0.pos.y, min(v1.pos.y, v2.pos.y)) == v1.pos.y) {
			if (v0.pos.y <= v2.pos.y) {
				interpolate(pixel.first, pixel.second, v1, v0, v2);
			}
			else {
				interpolate(pixel.first, pixel.second, v1, v2, v0);
			}
		}
		else if (min(v0.pos.y, min(v1.pos.y, v2.pos.y)) == v2.pos.y) {
			if (v0.pos.y <= v1.pos.y) {
				interpolate(pixel.first, pixel.second, v2, v0, v1);
			}
			else {
				interpolate(pixel.first, pixel.second, v2, v1, v0);
			}
		}*/
		
	}
	/*for (i; i < pixels.size(); i++) {
		interpolate(pixels[normals.size()].first, pixels[normals.size()].second, v0, v1, v2);
	}*/
	// -----------------------------------------------------------------------------------------

	// pixel shader ----------------------------------------------------------------------------
	for (int i = 0; i < pixels.size(); i++) {
		//framebuffer.colorBuffer.push_back(pixelShader(uvCoords[i], normals[i], texture.data));
	}
	// TODO: colorBuffer fylld, kan lägga till den som textur..? where??


	// -----------------------------------------------------------------------------------------
}

void Renderer::draw(void* handle)
{
	Buffers* buffers = (Buffers*)handle;
	pixels.clear();
	normals.clear();
	uvCoords.clear();
	framebuffer.colorBuffer.resize(framebuffer.width * framebuffer.height);

	for (int i = 0; i < buffers->indexBuffer.size(); i += 3) {
		rasterizeTriangle(buffers->vertexBuffer[i], buffers->vertexBuffer[i + 1], buffers->vertexBuffer[i + 2]);
	}



}

void Renderer::setVertexShader(const std::function<Vec4(Vertex&)> vertexShader) {
	this->vertexShader = vertexShader;
}
void Renderer::setPixelShader(const std::function<Vec4(Vec2, Vec3, unsigned char*)> pixelShader) {
	this->pixelShader = pixelShader;
}

void Renderer::loadTextureFile(const char* fileName) {
	texture.data = stbi_load(fileName, &texture.width, &texture.height, &texture.comp, STBI_rgb_alpha);
	//textureColor = stbi_load(fileName, &textureWidth, &textureHeight, &channels, STBI_rgb_alpha);
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
	std::vector<Vec2> lines[] = { line0, line1, line2 };
	int shortest, longest, top1, top2, thirdLine, left, right, currentY, currentX, Xend;
	// check which lines share the top-most vertex (lowest y) --------------------------------------
	// assign value to top1, it's the line with lowest y
	top1 = min(lines[0][0].y, min(lines[1][0].y, lines[2][0].y));
	if (top1 == lines[0][0].y) 
		top1 = 0;
	else if (top1 == lines[1][0].y)
		top1 = 1;
	else if (top1 == lines[2][0].y)
		top1 = 2;

	// assign value to top2, lines[top1][0].y needs to be the same as lines[top2][0].y
	switch (top1) {
	case 0:
		if (lines[top1][0].y == lines[1][0].y)
			top2 = 1;
		else
			top2 = 2;
		break;
	case 1:
		if (lines[top1][0].y == lines[0][0].y)
			top2 = 0;
		else
			top2 = 2;
		break;
	case 2:
		if (lines[top1][0].y == lines[0][0].y)
			top2 = 0;
		else
			top2 = 1;
		break;
	}
	// ---------------------------------------------------------------------------------------------

	// check which of those two lines are shortest, lines[top1] or lines[top2] ---------------------
	if (lines[top1][lines[top1].size() - 1].y <= lines[top2][lines[top2].size() - 1].y) {
		shortest = top1;
		longest = top2;
	}
	else {
		shortest = top2;
		longest = top1;
	}
	// ---------------------------------------------------------------------------------------------

	// loop trough shortest line to fill between the two lines -------------------------------------
	// check which line is on the left, start from there
	if (lines[top1][lines[top1].size() - 1].x <= lines[top2][lines[top2].size() - 1].x) {
		left = top1;
		right = top2;
	}
	else {
		left = top2;
		right = top1;
	}

	// fill pixels until currentY is the same as the last y in shortest line
	int leftIndex = 0, rightIndex = 0;
	currentY = lines[left][leftIndex].y;
	while (currentY < lines[shortest][lines[shortest].size() - 1].y)
	{
		// beginning of each row, find end of row on lines[right], update Xend
		currentX = lines[left][leftIndex].x;
		// stop incrementing index when rightIndex is at the last index or when rightIndex is on the next line
		while (lines[right][rightIndex].y == currentY) 
		{
			rightIndex++;
			if (rightIndex == lines[right].size())
				break; // don't check while condition cause rightIndex is now out of bounds, abort loop
		}
		Xend = lines[right][rightIndex - 1].x;
		
		// beginning of each row's X, add to pixels
		pixels.push_back(std::make_pair(currentX, currentY));
		while (currentX < Xend) 
		{
			currentX++;
			pixels.push_back(std::make_pair(currentX, currentY));
		}

		// end of each row, update leftIndex
		currentY++;
		while (lines[left][leftIndex].y != currentY) 
		{
			leftIndex++;
			if (leftIndex == lines[left].size())
				break;
		}
	}
	// don't fill last row, let thirdLine do it

	
	// loop through the third line -----------------------------------------------------------------
	// find thirdLine, keep longest
	if (longest == 0) {
		if (shortest == 1) {
			thirdLine = 2;
		}
		else {
			thirdLine = 1;
		}
	}
	else if (longest == 1) {
		if (shortest == 0) {
			thirdLine = 2;
		}
		else {
			thirdLine = 0;
		}
	}
	else if (longest == 2) {
		if (shortest == 0) {
			thirdLine = 1;
		}
		else {
			thirdLine = 0;
		}
	}
	//reset left or right to 0 depending on which is shorter, update shortest
	if (left == shortest) {
		left = thirdLine;
		leftIndex = 0;
	}
	else {
		right = thirdLine;
		rightIndex = 0;
	}
	shortest = thirdLine;

	// loop, fill rest of triangle, starting from lines[shortest][0] which is on last row of first flat bottom triangle
	// i.e. fill the rest of the triangle, do not allow loop to skip first row this time, <= instead of <
	while (currentY <= lines[shortest][lines[shortest].size() - 1].y)
	{
		// beginning of each row, find end of row on lines[right], update Xend
		currentX = lines[left][leftIndex].x;
		while (lines[right][rightIndex].y == currentY) 
		{
			rightIndex++;
			if (rightIndex == lines[right].size())
				break;
		}
		Xend = lines[right][rightIndex - 1].x;

		// beginning of each row's X, add to pixels
		pixels.push_back(std::make_pair(currentX, currentY));
		while (currentX < Xend) 
		{
			currentX++;
			pixels.push_back(std::make_pair(currentX, currentY));
		}

		// end of each row, update leftIndex
		currentY++;
		while (lines[left][leftIndex].y != currentY) 
		{
			leftIndex++;
			if (leftIndex == lines[left].size())
				break;
		}
	}
	// ---------------------------------------------------------------------------------------------
}

void Renderer::interpolate(int x, int y, Vertex& v0, Vertex& v1, Vertex& v2)
{
	// räkna ut vikterna för varje pixel, använd dem för att lägga till värde i alla attributes
	float w0, w1, w2, denominator;
	denominator = (v1.pos.y - v2.pos.y) * (v0.pos.x - v2.pos.x) + (v2.pos.x - v1.pos.x) * (v0.pos.y - v2.pos.y);
	w0 = ((v1.pos.y - v2.pos.y) * (x - v2.pos.x) + (v2.pos.x - v1.pos.x) * (y - v2.pos.y)) / denominator;
	w1 = ((v2.pos.y - v0.pos.y) * (x - v2.pos.x) + (v0.pos.x - v2.pos.x) * (y - v2.pos.y)) / denominator;
	w2 = 1 - w0 - w1;

	Vec3 normal;
	Vec2 uvcoord;
	// interpolate normal values
	normal.x = w0 * v0.normal.x + w1 * v1.normal.x + w2 * v2.normal.x;
	normal.y = w0 * v0.normal.y + w1 * v1.normal.y + w2 * v2.normal.y;
	normal.z = w0 * v0.normal.z + w1 * v1.normal.z + w2 * v2.normal.z;
	normals.push_back(normal);

	// interpolate uvcoord values, need to be integers for texture coordinates
	uvcoord.x = round(w0 * v0.uv.x + w1 * v1.uv.x + w2 * v2.uv.x);
	uvcoord.y = round(w0 * v0.uv.y + w1 * v1.uv.y + w2 * v2.uv.y);
	uvCoords.push_back(uvcoord);

	// later: interpolate depth values
}

float Renderer::min(float a, float b)
{
	if (a <= b) {
		return a;
	}
	else
		return b;
}


