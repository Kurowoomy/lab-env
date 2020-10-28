#include "config.h"
#include "Renderer.h"


void* Renderer::addVertexIndexBuffer(const char* objPath)
{
	if (buffers.indexBuffer.empty()) {
		// read objFile -----------------------------------------------------------------------------------
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
		// ------------------------------------------------------------------------------------------------

		// make mesh --------------------------------------------------------------------------------------
		MeshResource mesh; // (could probably insert data directly to v instead of making a mesh just to copy it over to v)
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
		// write all values into the buffers in this renderer class
		for (int i = 0; i < vertexIndices.size(); i++) {
			buffers.indexBuffer.push_back(i);
			Vertex v = Vertex(mesh.vertices[i], mesh.normals[i], mesh.uvs[i]);
			buffers.vertexBuffer.push_back(v);
		}
		// ------------------------------------------------------------------------------------------------
	}

	return &buffers;
}

void Renderer::setFramebuffer(unsigned int width, unsigned int height) 
{
	// framebuffer setup ------------------------------------------------------------------------------
	framebuffer.width = width;
	framebuffer.height = height;
	framebuffer.colorBuffer.resize(framebuffer.width * framebuffer.height);
	framebuffer.depthBuffer.resize(framebuffer.width * framebuffer.height);
	// ------------------------------------------------------------------------------------------------
	
	// OpenGL setup -----------------------------------------------------------------------------------
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);

	texture.genTexture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	texture.unbindTexture();
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.textureID, 0);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("D: Framebuffer incomplete.");
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	// ------------------------------------------------------------------------------------------------
}
void Renderer::bindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
}
void Renderer::unbindFramebuffer() {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Renderer::rasterizeTriangle(Vertex v0, Vertex v1, Vertex v2) 
{
	// use vertex shader on the three vertices
	Vec4 newPos0 = vertexShader(v0);
	Vec4 newPos1 = vertexShader(v1);
	Vec4 newPos2 = vertexShader(v2);

	// convert x and y to raster space, pixel positions but with decimals
	v0.pos = convertToRasterSpace(newPos0);
	v1.pos = convertToRasterSpace(newPos1);
	v2.pos = convertToRasterSpace(newPos2);

	// create the lines, made of actual pixel positions in integer format -----------------------------
	std::vector<Vec2> line0 = createLine(v0.pos.x, v1.pos.x, v0.pos.y, v1.pos.y);
	std::vector<Vec2> line1 = createLine(v1.pos.x, v2.pos.x, v1.pos.y, v2.pos.y);
	std::vector<Vec2> line2 = createLine(v2.pos.x, v0.pos.x, v2.pos.y, v0.pos.y);

	// sort y values to make [0].y lowest [size()].y highest
	if (line0[0].y > line0[line0.size() - 1].y) {
		std::reverse(line0.begin(), line0.end());
	}
	if (line1[0].y > line1[line1.size() - 1].y) {
		std::reverse(line1.begin(), line1.end());
	}
	if (line2[0].y > line2[line2.size() - 1].y) {
		std::reverse(line2.begin(), line2.end());
	}

	// draw the lines of the triangle
	/*drawLine(line0);
	drawLine(line1);
	drawLine(line2);*/
	// ------------------------------------------------------------------------------------------------
	
	// find all pixels and their attributes -----------------------------------------------------------
	// fill triangle and interpolate vertex attributes
	pixels.clear();
	normals.clear();
	uvCoords.clear();
	fillTriangle(line0, line1, line2);
	//pixels vector is ready to use for interpolation!
	int i = 0;
	for (Vec3 pixel : pixels) {
		interpolate(pixel.x, pixel.y, i, v0, v1, v2);
		i++;
	}
	// ------------------------------------------------------------------------------------------------

	// pixel shader -----------------------------------------------------------------------------------
	for (int i = 0; i < pixels.size(); i++) {
		// view frustum check
		if (pixels[i].y < 0 || pixels[i].y > framebuffer.height - 1 || pixels[i].x < 0 || pixels[i].x > framebuffer.width - 1
			|| pixels[i].z < near || pixels[i].z > far) {
			// don't fill pixel if it's outside of view space
			continue;
		}
		// passed view frustum check. Next: depth buffer check.
		// i.e. if currentPixel.pos.z < depthbuffer[currentPixelPos], update depthbuffer and add pixel to colorbuffer
		else if (pixels[i].z < framebuffer.depthBuffer[framebuffer.width * pixels[i].y + pixels[i].x]) 
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
	// ------------------------------------------------------------------------------------------------
}

void Renderer::draw(void* handle)
{
	// depth buffer preparations, set all to far clipping plane
	for (int i = 0; i < framebuffer.depthBuffer.size(); i++) {
		framebuffer.depthBuffer[i] = far;
	}

	// colorbuffer preparations ---------------------------------------------------------------------
	// background color
	for (int i = 0; i < framebuffer.colorBuffer.size(); i++) {
		framebuffer.colorBuffer[i].x = 0;
		framebuffer.colorBuffer[i].y = 0;
		framebuffer.colorBuffer[i].z = 0;
		framebuffer.colorBuffer[i].w = 1;
	}
	// background line
	for (int i = 0; i < 3000; i++) {
		framebuffer.colorBuffer[i].x = 0;
		framebuffer.colorBuffer[i].y = 255 / 255;
		framebuffer.colorBuffer[i].z = 0;
		framebuffer.colorBuffer[i].w = 255 / 255;
	}
	// background line
	for (int i = framebuffer.height * framebuffer.width / 2; i < framebuffer.height * framebuffer.width / 2 + 3000; i++) {
		framebuffer.colorBuffer[i].x = 255.0f / 255;
		framebuffer.colorBuffer[i].y = 210.0f / 255;
		framebuffer.colorBuffer[i].z = 236.0f / 255;
		framebuffer.colorBuffer[i].w = 255.0f / 255;
	}
	//-----------------------------------------------------------------------------------------------

	Buffers* buffers = (Buffers*)handle;
	for (int i = 0; i < buffers->indexBuffer.size(); i += 3) { // += 3 because object is triangulated, has three vertices per face
		rasterizeTriangle(buffers->vertexBuffer[i], buffers->vertexBuffer[i + 1], buffers->vertexBuffer[i + 2]);
	}

	// colorbuffer is now filled and ready to render to frame
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
	int dx, dy, x, y, end, fault;
	dx = x1 - x0;
	dy = y1 - y0;

	if (abs(dy) <= abs(dx)) // increment x for each pixel
	{
		if (dx >= 0) // go from x0 to x1
		{
			x = x0;
			y = y0;
			end = x1;
		}
		else // go from x1 to x0
		{
			x = x1;
			y = y1;
			end = x0;
		}

		// go to the right, x++
		newLine.push_back(Vec2(x, y));
		fault = 2 * abs(dy) - abs(dx);
		for (int i = 0; x < end; i++)
		{
			x++;

			if (fault < 0) { // y is still close to the line
				fault += 2 * abs(dy);
			}
			else // y is too far from the line
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
					y++; // slope goes down
				}
				else {
					y--; // slope goes up
				}
				fault += 2 * (abs(dy) - abs(dx));
			}

			newLine.push_back(Vec2(x, y));
		}
	}
	else // increment y for each pixel
	{ 
		if (dy >= 0) // go from y0 to y1
		{
			x = x0;
			y = y0;
			end = y1;
		}
		else // go from y1 to y0
		{
			x = x1;
			y = y1;
			end = y0;
		}

		// go down, y++
		newLine.push_back(Vec2(x, y));
		fault = 2 * abs(dx) - abs(dy);
		for (int i = 0; y < end; i++) {
			y++;

			if (fault < 0) {
				fault += 2 * abs(dx);
			}
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
					x++;
				}
				else {
					x--;
				}
				fault += 2 * (abs(dx) - abs(dy));
			}

			newLine.push_back(Vec2(x, y));
		}
	}

	return newLine;
}

void Renderer::drawLine(std::vector<Vec2> line)
{
	for (Vec2 linePixel : line) {
		// view frustum check
		if (linePixel.y < 0 || linePixel.y > framebuffer.height - 1 || linePixel.x < 0 || linePixel.x > framebuffer.width - 1) {
			// don't fill pixel if it's outside of view space
			continue;
		}
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].x = 1;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].y = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].z = 0;
		framebuffer.colorBuffer[linePixel.y * framebuffer.width + linePixel.x].w = 1;
	}
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

	// here we can flip the model
	Vec2 windowSpacePos = Vec2((pos.x + 1) / 2 * framebuffer.width, (pos.y + 1) / 2 * framebuffer.height);
	pos.x = windowSpacePos.x;
	pos.y = windowSpacePos.y;

	return pos;
}

void Renderer::fillTriangle(std::vector<Vec2> line0, std::vector<Vec2> line1, std::vector<Vec2> line2)
{
	// (new tactic)
	// (save all rows, don't bother with flat top or bottom)
	// (just need to set which line is the longest, and fill row for each y pixel)

	// map y pixel to its pair of lowest and highest x pixels, that's all data we need to fill a row with pixels
	std::map<int, std::pair<int, int>> allXonY;

	// find longest line to know how many rows there are
	// then allocate space in map, with corresponding y value of each row as key
	int dy0 = line0[line0.size() - 1].y - line0[0].y;
	int dy1 = line1[line1.size() - 1].y - line1[0].y;
	int dy2 = line2[line2.size() - 1].y - line2[0].y;
	int maxY = max(dy0, max(dy1, dy2));
	std::pair<int, int> xPair = std::make_pair<int, int> ((int)framebuffer.width, 0);
	if (maxY == dy0) {
		for (int i = line0[0].y; i <= line0[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::pair<int, int>>(i, xPair));
		}
	}
	else if (maxY == dy1) {
		for (int i = line1[0].y; i <= line1[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::pair<int, int>>(i, xPair));
		}
	}
	else if (maxY == dy2) {
		for (int i = line2[0].y; i <= line2[0].y + maxY; i++) {
			allXonY.insert(std::pair<int, std::pair<int, int>>(i, xPair));
		}
	}
	
	// all y values are now found
	// find lowest and highest value for each row
	for (int i = 0; i < line0.size(); i++) {
		if (line0[i].x < allXonY[line0[i].y].first) {
			allXonY[line0[i].y].first = line0[i].x;
		}
		if (line0[i].x > allXonY[line0[i].y].second) {
			allXonY[line0[i].y].second = line0[i].x;
		}
	}
	for (int i = 0; i < line1.size(); i++) {
		if (line1[i].x < allXonY[line1[i].y].first) {
			allXonY[line1[i].y].first = line1[i].x;
		}
		if (line1[i].x > allXonY[line1[i].y].second) {
			allXonY[line1[i].y].second = line1[i].x;
		}
	}
	for (int i = 0; i < line2.size(); i++) {
		if (line2[i].x < allXonY[line2[i].y].first) {
			allXonY[line2[i].y].first = line2[i].x;
		}
		if (line2[i].x > allXonY[line2[i].y].second) {
			allXonY[line2[i].y].second = line2[i].x;
		}
	}
	
	// fill all rows with pixels
	for (int i = allXonY.begin()->first; i <= allXonY.begin()->first + maxY; i++) {
		fillRow(allXonY[i].first, allXonY[i].second, i);
	}
}

void Renderer::fillRow(int x0, int x1, int y) {
	while (x0 <= x1) {
		pixels.push_back(Vec3(x0, y, 0));
		x0++;
	}
}

void Renderer::interpolate(int x, int y, int i, Vertex& v0, Vertex& v1, Vertex& v2)
{
	// calculate the weights for each pixel, use them to add value to all attributes
	float w0 = 0, w1 = 0, w2;
	Vec3 P = Vec3(x, y, 0);
	Vec3 v0v1 = v1.pos - v0.pos;
	Vec3 v0v2 = v2.pos - v0.pos;

	Vec3 v0v1_xy = Vec3(v0v1.x, v0v1.y, 0);
	Vec3 v0v2_xy = Vec3(v0v2.x, v0v2.y, 0);

	float denominator = v0v1_xy.crossProduct(v0v2_xy).length(); // parallellogram for whole triangle
	if (denominator == 0) {
		printf("denominator is zero somebody do something about it D:");
	}
	else {
		Vec3 v1v2 = v2.pos - v1.pos;
		Vec3 v1toP = P - v1.pos;

		Vec3 v1v2_xy = Vec3(v1v2.x, v1v2.y, 0);
		Vec3 v1toP_xy = Vec3(v1toP.x, v1toP.y, 0);
		w0 = v1v2_xy.crossProduct(v1toP_xy).length() / denominator;

		v0v2 = v2.pos - v0.pos;
		Vec3 v0toP = P - v0.pos;

		Vec3 v0v2_xy = Vec3(v0v2.x, v0v2.y, 0);
		Vec3 v0toP_xy = Vec3(v0toP.x, v0toP.y, 0);
		w1 = v0v2_xy.crossProduct(v0toP_xy).length() / denominator;
	}
	w2 = 1 - w0 - w1;

	// precision error fix
	if (w1 >= 1) {
		w2 = 0;
		w0 = 0;
		w1 = 1;
	}
	if (w0 >= 1) {
		w0 = 1;
		w1 = 0;
		w2 = 0;
	}
	if (w2 >= 1) {
		w2 = 1;
		w0 = 0;
		w1 = 0;
	}
	
	Vec3 normal;
	Vec2 uvcoord;

	// interpolate normal values
	normal.x = w0 * v0.normal.x + w1 * v1.normal.x + w2 * v2.normal.x;
	normal.y = w0 * v0.normal.y + w1 * v1.normal.y + w2 * v2.normal.y;
	normal.z = w0 * v0.normal.z + w1 * v1.normal.z + w2 * v2.normal.z;
	normals.push_back(normal);

	// interpolate uvcoord values, need to be integers for texture coordinates
	uvcoord.x = round(w0 * v0.uv.x + w1 * v1.uv.x + w2 * v2.uv.x);
	// make sure there are no precision errors
	if (uvcoord.x < 0) { // händer vid transformering
		uvcoord.x = 0;
	}
	if (uvcoord.x > texture.width) { // händer vid rotation
		uvcoord.x = texture.width;
	}
	uvcoord.y = round(w0 * v0.uv.y + w1 * v1.uv.y + w2 * v2.uv.y);
	if (uvcoord.y < 0) { // händer vid transformering
		uvcoord.y = 0;
	}
	if (uvcoord.y > texture.width) { // händer vid rotation
		uvcoord.y = texture.width; 
	}
	uvCoords.push_back(uvcoord);

	// interpolate depth values, add to depth buffer
	pixels[i].z = w0 * v0.pos.z + w1 * v1.pos.z + w2 * v2.pos.z;
}

float Renderer::min(float a, float b)
{
	if (a <= b) {
		return a;
	}
	else
		return b;
}
float Renderer::max(float a, float b) 
{
	if (a >= b) {
		return a;
	}
	else
		return b;
}
float Renderer::pow(float a, float b) 
{
	float result = 1;
	for (int i = 0; i < b; i++) {
		result *= a;
	}
	return result;
}

void Renderer::setTransform(Matrix4 newTransform) {
	model = newTransform;
}
Matrix4& Renderer::getTransform() {
	return model;
}


