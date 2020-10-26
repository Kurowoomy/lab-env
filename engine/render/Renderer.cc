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
	framebuffer.colorBuffer.resize(framebuffer.width * framebuffer.height);
	framebuffer.depthBuffer.resize(framebuffer.width * framebuffer.height);
	
	//glEnable(GL_TEXTURE_2D);
	texture.genTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	texture.unbindTexture();

	glGenRenderbuffers(1, &depthID);
	glBindRenderbuffer(GL_RENDERBUFFER, depthID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);

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
	/*if (newPos0.x == 0) {
		printf("noll :c");
	}*/
	v0.pos = convertToRasterSpace(newPos0);
	v1.pos = convertToRasterSpace(newPos1);
	v2.pos = convertToRasterSpace(newPos2);

	/*v0.pos.x = round(v0.pos.x);
	v0.pos.y = round(v0.pos.y);
	v1.pos.x = round(v1.pos.x);
	v1.pos.y = round(v1.pos.y);
	v2.pos.x = round(v2.pos.x);
	v2.pos.y = round(v2.pos.y);*/
	// -----------------------------------------------------------------------------------------

	// Måste först hitta alla pixlar man behöver jobba med för denna triangel-------------------
	// dvs börja använda pos.z! Ju närmare kameran desto närmare 0, ju längre bort desto större z-värde. Bakom kameran är det negativa värden.
	
	
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

	// draw the lines
	/*for (Vec2 linePixel : line0) {
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].x = 1;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].y = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].z = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].w = 1;
	}
	for (Vec2 linePixel : line1) {
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].x = 1;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].y = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].z = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].w = 1;
	}
	for (Vec2 linePixel : line2) {
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].x = 1;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].y = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].z = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].w = 1;
	}*/

	// fill triangle and interpolate vertex attributes
	// use these three only for this triangle and pixel shader
	pixels.clear();
	normals.clear();
	uvCoords.clear();
	fillTriangle(line0, line1, line2); 
	//pixels vector is ready to use for interpolation!
	int i = 0;
	std::vector<int> toErase;
	/*if (pixels.size() == 0) {
		printf("triangle not filled");
	}*/
	for (Vec3 pixel : pixels) {
		// make all pos into int because they're coordinates
		
		/*v0.pos.x = (int)v0.pos.x;
		v0.pos.y = (int)v0.pos.y;
		v1.pos.x = (int)v1.pos.x;
		v1.pos.y = (int)v1.pos.y;
		v2.pos.x = (int)v2.pos.x;
		v2.pos.y = (int)v2.pos.y;*/

		interpolate(pixel.x, pixel.y, i, toErase, v0, v1, v2);
		i++;
	}
	/*int index = 0;
	for (int i : toErase) {
		i += index;
		pixels.erase(pixels.begin() + i);
		index--;
	}
	toErase.clear();*/
	// -----------------------------------------------------------------------------------------

	// pixel shader ----------------------------------------------------------------------------
	for (int i = 0; i < pixels.size(); i++) {
		// depth buffer check!
		// dvs om currentPixel.pos.z < depthbuffer[currentPixelPos], uppdatera depthbuffer och måla pixel/lägg till pixel i colorbuffer
		if (pixels[i].z < framebuffer.depthBuffer[framebuffer.width * pixels[i].y + pixels[i].x]) 
		{
			framebuffer.depthBuffer[framebuffer.width * pixels[i].y + pixels[i].x] = pixels[i].z;

			Vec4 pixel = pixelShader(uvCoords[i], normals[i], texture.data);
			framebuffer.colorBuffer[pixels[i].y * framebuffer.width + pixels[i].x].x = pixel.x / 255;
			framebuffer.colorBuffer[pixels[i].y * framebuffer.width + pixels[i].x].y = pixel.y / 255;
			framebuffer.colorBuffer[pixels[i].y * framebuffer.width + pixels[i].x].z = pixel.z / 255;
			framebuffer.colorBuffer[pixels[i].y * framebuffer.width + pixels[i].x].w = pixel.w / 255;
		}
		// else, do nothing for this pixel. Don't change pixel color in colorBuffer and let depthBuffer keep its value for this pixel.
	}
	// -----------------------------------------------------------------------------------------
}

void Renderer::draw(void* handle)
{
	Buffers* buffers = (Buffers*)handle;

	for (int i = 0; i < framebuffer.depthBuffer.size(); i++) { // initialize depth buffer as far away from camera as possible
		// ska egentligen få viewport-värdet men fixar det senare
		framebuffer.depthBuffer[i] = 1000;
	}
	for (int i = 0; i < framebuffer.colorBuffer.size(); i++) {
		framebuffer.colorBuffer[i].x = 0;
		framebuffer.colorBuffer[i].y = 0;
		framebuffer.colorBuffer[i].z = 0;
		framebuffer.colorBuffer[i].w = 1;
	}
	for (int i = 0; i < 1000; i++) {
		framebuffer.colorBuffer[i].x = 0;
		framebuffer.colorBuffer[i].y = 255 / 255;
		framebuffer.colorBuffer[i].z = 0;
		framebuffer.colorBuffer[i].w = 255 / 255;
	}
	for (int i = framebuffer.height * framebuffer.width / 2; i < framebuffer.height * framebuffer.width / 2 + 3000; i++) {
		framebuffer.colorBuffer[i].x = 255.0f / 255;
		framebuffer.colorBuffer[i].y = 210.0f / 255;
		framebuffer.colorBuffer[i].z = 236.0f / 255;
		framebuffer.colorBuffer[i].w = 255.0f / 255;
	}

	for (int i = 0; i < buffers->indexBuffer.size(); i += 3) {
		rasterizeTriangle(buffers->vertexBuffer[i], buffers->vertexBuffer[i + 1], buffers->vertexBuffer[i + 2]);
	}
	std::reverse(framebuffer.colorBuffer.begin(), framebuffer.colorBuffer.end());

	// colorbuffer should now be filled and ready to render to frame
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
	texture.bindTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer.width, framebuffer.height, 0, GL_RGBA, GL_FLOAT, &framebuffer.colorBuffer[0]);
	texture.unbindTexture();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::setVertexShader(const std::function<Vec4(Vertex&)> vertexShader) {
	this->vertexShader = vertexShader;
}
void Renderer::setPixelShader(const std::function<Vec4(Vec2, Vec3, unsigned char*)> pixelShader) {
	this->pixelShader = pixelShader;
}

void Renderer::loadTextureFile(const char* fileName) {
	texture.data = stbi_load(fileName, &texture.width, &texture.height, &texture.comp, STBI_rgb_alpha);
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
		pos = Vec3(v.x / v.w, v.y / v.w, v.z);
	}
	else {
		printf("w is 0 :( \n");
	}

	Vec2 windowSpacePos = Vec2((pos.x + 1) / 2 * framebuffer.width, (1 - pos.y) / 2 * framebuffer.height);
	/*if (windowSpacePos.x == 512) {
		printf("??");
	}*/
	pos.x = windowSpacePos.x;
	pos.y = windowSpacePos.y;

	return pos;
}

void Renderer::fillTriangle(std::vector<Vec2> line0, std::vector<Vec2> line1, std::vector<Vec2> line2)
{
	// new tactic
	// save all rows, don't bother with flat top or bottom
	// just need to set which line is the longest, and fill row for each y pixel

	// map y pixel to all its x pixels
	std::map<int, std::vector<int>> allXonY;

	// skapa alla y keys i allXonY
	int dy0 = line0[line0.size() - 1].y - line0[0].y;
	int dy1 = line1[line1.size() - 1].y - line1[0].y;
	int dy2 = line2[line2.size() - 1].y - line2[0].y;
	int maxY = max(dy0, max(dy1, dy2));
	if (maxY == dy0) {
		for (int i = line0[0].y; i <= line0[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::vector<int>>(i, std::vector<int>()));
		}
	}
	else if (maxY == dy1) {
		for (int i = line1[0].y; i <= line1[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::vector<int>>(i, std::vector<int>()));
		}
	}
	else if (maxY == dy2) {
		for (int i = line2[0].y; i <= line2[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::vector<int>>(i, std::vector<int>()));
		}
	}
	
	// nu har vi alla y-värden i allXonY
	// fyll map för varje linje
	//int lowest = 0, highest = 0;
	for (int i = 0; i < line0.size(); i++) {
		// gör inget om det blir duplikationer av x i vektorerna. Ska ändå bara använda första och sista efter sortering.
		allXonY[line0[i].y].push_back(line0[i].x);
		/*if (line0[i].x < lowest) {
			lowest = line0[i].x;
		}
		else if (line0[i].x > highest) {
			highest = line0[i].x;
		}*/
	}
	for (int i = 0; i < line1.size(); i++) {
		allXonY[line1[i].y].push_back(line1[i].x);
		/*if (line1[i].x < lowest) {
			lowest = line1[i].x;
		}
		else if (line1[i].x > highest) {
			highest = line1[i].x;
		}*/
	}
	for (int i = 0; i < line2.size(); i++) {
		allXonY[line2[i].y].push_back(line2[i].x);
		/*if (line2[i].x < lowest) {
			lowest = line2[i].x;
		}
		else if (line2[i].x > highest) {
			highest = line2[i].x;
		}*/
	}
	
	// sortera varje x-vektor i allXonY
	for (int i = allXonY.begin()->first; i <= allXonY.begin()->first + maxY; i++) {
		std::sort(allXonY[i].begin(), allXonY[i].end()); // probably slows down rendering a lot, should find and save lowest and highest x instead
		fillRow(allXonY[i][0], allXonY[i][allXonY[i].size() - 1], i);
		//fillRow(lowest, highest, i);
	}

	
	{
	//std::vector<Vec2> lines[] = { line0, line1, line2 };
	//int shortest, longest, top1, top2, left, right, currentY, currentX, Xend;
	//// check which lines share the top-most vertex (lowest y) --------------------------------------
	//// assign value to top1, it's the line with lowest y

	//// case when one top line is same y from start to end, all three lines then share the same lowest y
	//if (lines[0][0].y == lines[1][0].y && lines[1][0].y == lines[2][0].y) {
	//	// doesn't matter which line is longest, set longest to one of the remaining indexes after shortest is found.
	//	if (lines[0][0].y == lines[0][lines[0].size() - 1].y) {
	//		shortest = 0;
	//		longest = 1;
	//	}
	//	else if (lines[1][0].y == lines[1][lines[1].size() - 1].y) {
	//		shortest = 1;
	//		longest = 0;
	//	}
	//	else {
	//		shortest = 2;
	//		longest = 0;
	//	}
	//}
	//// all the other cases
	//else {
	//	top1 = min(lines[0][0].y, min(lines[1][0].y, lines[2][0].y));
	//	if (top1 == lines[0][0].y)
	//		top1 = 0;
	//	else if (top1 == lines[1][0].y)
	//		top1 = 1;
	//	else if (top1 == lines[2][0].y)
	//		top1 = 2;

	//	// assign value to top2, lines[top1][0].y needs to be the same as lines[top2][0].y
	//	switch (top1) {
	//	case 0:
	//		if (lines[top1][0].y == lines[1][0].y)
	//			top2 = 1;
	//		else
	//			top2 = 2;
	//		break;
	//	case 1:
	//		if (lines[top1][0].y == lines[0][0].y)
	//			top2 = 0;
	//		else
	//			top2 = 2;
	//		break;
	//	case 2:
	//		if (lines[top1][0].y == lines[0][0].y)
	//			top2 = 0;
	//		else
	//			top2 = 1;
	//		break;
	//	}
	//	// ---------------------------------------------------------------------------------------------

	//	// check which of those two lines are shortest, lines[top1] or lines[top2] ---------------------
	//	if (lines[top1][lines[top1].size() - 1].y <= lines[top2][lines[top2].size() - 1].y) {
	//		shortest = top1;
	//		longest = top2;
	//	}
	//	else {
	//		shortest = top2;
	//		longest = top1;
	//	}
	//}
	//// ---------------------------------------------------------------------------------------------

	//// loop trough shortest line to fill between the two lines -------------------------------------
	//// check which line is on the left, start from there
	//// check end of shortest line! 
	//// for first triangle, end means the second last row for shortest line, not last row. But for left/right check we can use last row.
	//int lastY = lines[shortest][lines[shortest].size() - 1].y;
	//// find "last" index for longest line
	//int longLineIndex = 0;
	//if (lines[longest][lines[longest].size() - 1].y == lastY) {
	//	longLineIndex = lines[longest].size();
	//}
	//else {
	//	while (lines[longest][longLineIndex].y <= lastY) {
	//		longLineIndex++;
	//	}
	//}
	//// (longLineIndex - 1) is the "last" index of the longest line, on the same row as last row for shortest line.
	//if (lines[longest][longLineIndex - 1].x <= lines[shortest][lines[shortest].size() - 1].x) { // unfinished
	//	left = longest;
	//	right = shortest;
	//}
	//else {
	//	left = shortest;
	//	right = longest;
	//}

	//// fill pixels until currentY is the same as the last y in shortest line
	//int leftIndex = 0, rightIndex = 0;
	//currentY = lines[left][leftIndex].y;
	//while (currentY < lines[shortest][lines[shortest].size() - 1].y)
	//{
	//	// beginning of each row, find end of row on lines[right], update Xend
	//	currentX = lines[left][leftIndex].x;
	//	// stop incrementing index when rightIndex is at the last index or when rightIndex is on the next line
	//	while (lines[right][rightIndex].y == currentY) 
	//	{
	//		rightIndex++;
	//		if (rightIndex == lines[right].size())
	//			break; // don't check while condition cause rightIndex is now out of bounds, abort loop
	//	}
	//	Xend = lines[right][rightIndex - 1].x;
	//	
	//	// beginning of each row's X, add to pixels
	//	pixels.push_back(Vec3(currentX, currentY, 0));
	//	while (currentX < Xend) 
	//	{
	//		currentX++;
	//		pixels.push_back(Vec3(currentX, currentY, 0));
	//	}

	//	// end of each row, update leftIndex
	//	currentY++;
	//	while (lines[left][leftIndex].y != currentY) 
	//	{
	//		leftIndex++;
	//		if (leftIndex == lines[left].size())
	//			break;
	//	}
	//}
	//// don't fill last row, let thirdLine do it

	//
	//// loop through the third line -----------------------------------------------------------------
	//// find third line, update shortest to third line, keep longest
	//if (longest == 0) {
	//	if (shortest == 1) {
	//		shortest = 2;
	//	}
	//	else {
	//		shortest = 1;
	//	}
	//}
	//else if (longest == 1) {
	//	if (shortest == 0) {
	//		shortest = 2;
	//	}
	//	else {
	//		shortest = 0;
	//	}
	//}
	//else if (longest == 2) {
	//	if (shortest == 0) {
	//		shortest = 1;
	//	}
	//	else {
	//		shortest = 0;
	//	}
	//}
	//// it is guaranteed that longest line is on the same side as before
	//// reset left or right to 0 depending on which is shorter
	//if (longest == right) {
	//	left = shortest;
	//	leftIndex = 0;
	//}
	//else {
	//	right = shortest;
	//	rightIndex = 0;
	//}

	//// loop, fill rest of triangle, starting from lines[shortest][0] which is on last row of first flat bottom triangle
	//// i.e. fill the rest of the triangle, do not allow loop to skip first row this time, <= instead of <
	//while (currentY <= lines[shortest][lines[shortest].size() - 1].y)
	//{
	//	// beginning of each row, find end of row on lines[right], update Xend
	//	currentX = lines[left][leftIndex].x;
	//	while (lines[right][rightIndex].y == currentY) 
	//	{
	//		rightIndex++;
	//		if (rightIndex == lines[right].size())
	//			break;
	//	}
	//	Xend = lines[right][rightIndex - 1].x;

	//	// beginning of each row's X, add to pixels
	//	pixels.push_back(Vec3(currentX, currentY, 0));
	//	while (currentX < Xend) 
	//	{
	//		currentX++;
	//		pixels.push_back(Vec3(currentX, currentY, 0));
	//	}

	//	// end of each row, update leftIndex
	//	currentY++;
	//	while (lines[left][leftIndex].y != currentY) 
	//	{
	//		leftIndex++;
	//		if (leftIndex == lines[left].size())
	//			break;
	//	}
	//}
	// ---------------------------------------------------------------------------------------------
	}
}

void Renderer::fillRow(int x0, int x1, int y) {
	while (x0 <= x1) {
		pixels.push_back(Vec3(x0, y, 0));
		x0++;
	}
}

void Renderer::interpolate(int x, int y, int i, std::vector<int>& toErase, Vertex& v0, Vertex& v1, Vertex& v2)
{
	// räkna ut vikterna för varje pixel, använd dem för att lägga till värde i alla attributes
	float w0 = 0, w1 = 0, w2, denominator;
	denominator = (v1.pos.y - v2.pos.y) * (v0.pos.x - v2.pos.x) + (v2.pos.x - v1.pos.x) * (v0.pos.y - v2.pos.y);
	if (denominator == 0) {
		//printf("denominator is zero somebody do something about it D:");
	}
	else {
		w0 = ((v1.pos.y - v2.pos.y) * (x - v2.pos.x) + (v2.pos.x - v1.pos.x) * (y - v2.pos.y)) / denominator;
		w1 = ((v2.pos.y - v0.pos.y) * (x - v2.pos.x) + (v0.pos.x - v2.pos.x) * (y - v2.pos.y)) / denominator;
	}
	w2 = 1 - w0 - w1;
	// w2 blir negativ ibland pga väldigt små skillnader mellan w0 och w1.
	// om skillnaden är mindre än 1.0e-05, gör w2 till 0.
	//if (w2 < 0) {
	//	//w2 = 0;
	//}

	//if (w0 < 0 || w1 < 0 || w2 < 0) {
	//	//toErase.push_back(i);
	//}
	//else {
		Vec3 normal;
		Vec2 uvcoord;
		// interpolate normal values
		normal.x = w0 * v0.normal.x + w1 * v1.normal.x + w2 * v2.normal.x;
		normal.y = w0 * v0.normal.y + w1 * v1.normal.y + w2 * v2.normal.y;
		normal.z = w0 * v0.normal.z + w1 * v1.normal.z + w2 * v2.normal.z;
		normals.push_back(normal);

		// interpolate uvcoord values, need to be integers for texture coordinates
		uvcoord.x = round(w0 * v0.uv.x + w1 * v1.uv.x + w2 * v2.uv.x);
		if (uvcoord.x < 0) { // händer vid transformering
			//printf("what why is it negative D:");
			uvcoord.x = 0;
		}
		if (uvcoord.x > texture.width) { // händer vid rotation
			//printf("uv out of bounds");
			uvcoord.x = texture.width;
		}
		uvcoord.y = round(w0 * v0.uv.y + w1 * v1.uv.y + w2 * v2.uv.y);
		if (uvcoord.y < 0) { // händer vid transformering
			//printf("what why is it negative D:");
			uvcoord.y = 0;
		}
		if (uvcoord.y > texture.width) { // händer vid rotation
			//printf("uv out of bounds");
			uvcoord.y = texture.width; 
		}
		uvCoords.push_back(uvcoord);

		// add current triangle pixels to depthbuffer
		pixels[i].z = w0 * v0.pos.z + w1 * v1.pos.z + w2 * v2.pos.z;
	//}
}

float Renderer::min(float a, float b)
{
	if (a <= b) {
		return a;
	}
	else
		return b;
}
float Renderer::max(float a, float b) {
	if (a >= b) {
		return a;
	}
	else
		return b;
}
float Renderer::pow(float a, float b) {
	float result = 1;
	for (int i = 0; i < b; i++) {
		result *= a;
	}
	return result;
}

void Renderer::setTransform(Matrix4 newTransform)
{
	model = newTransform;
}

Matrix4& Renderer::getTransform()
{
	return model;
}


