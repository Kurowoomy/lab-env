#pragma once
#include "GL/glew.h"
#include "core/Math_Library.h"
#include <fstream>
#include <string>
#include <map>

class ShaderObject {
public:
	GLuint program;
	GLuint vertexShader;
	GLuint fragmentShader;
	std::map<std::string, int> uniformLocation;

	void generateVertexShader(const char* fileName);
	void generateFragmentShader(const char* fileName);
	void generateProgram();

	void makeUniform(std::string name);
	void uploadUniformMatrix4(std::string name, Matrix4 mat4);
	void uploadUniformVector4(std::string name, Vec4 vec4);
	void uploadUniformVector3(std::string name, Vec3 vec3);

	void useProgram();
	void quitProgram();
	void destroy();
};
