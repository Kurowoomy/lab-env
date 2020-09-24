#include "config.h"
#include "ShaderObject.h"

void ShaderObject::generateVertexShader(const char* fileName)
{
	// read file
	std::ifstream stream(fileName);
	const char* vschar;
	if (stream.is_open()) {
		std::string vsLine;
		std::string vs;
		while (getline(stream, vsLine)) {
			vs = vs + vsLine + '\n';
		}
		vschar = vs.c_str();

		// setup vertex shader
		this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLint length = static_cast<GLint>(std::strlen(vschar));
		glShaderSource(this->vertexShader, 1, &vschar, &length);
		glCompileShader(this->vertexShader);

		// get error log
		GLint shaderLogSize;
		glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}
	}
	else {
		printf("Vertex shader file could not be read.\n");
	}
}

void ShaderObject::generateFragmentShader(const char* fileName)
{
	std::ifstream stream(fileName);
	const char* fschar;
	if (stream.is_open()) {
		std::string fsLine;
		std::string fs;
		while (getline(stream, fsLine)) {
			fs = fs + fsLine + '\n';
		}
		fschar = fs.c_str();

		// setup pixel shader
		this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLint length = static_cast<GLint>(std::strlen(fschar));
		glShaderSource(this->fragmentShader, 1, &fschar, &length);
		glCompileShader(this->fragmentShader);

		// get error log
		GLint shaderLogSize;
		glGetShaderiv(this->fragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
		if (shaderLogSize > 0)
		{
			GLchar* buf = new GLchar[shaderLogSize];
			glGetShaderInfoLog(this->fragmentShader, shaderLogSize, NULL, buf);
			printf("[SHADER COMPILE ERROR]: %s", buf);
			delete[] buf;
		}
	}
	else {
		printf("Fragment shader file could not be read.\n");
	}
}

void ShaderObject::generateProgram()
{
	// create a program object
	this->program = glCreateProgram();
	glAttachShader(this->program, vertexShader);
	glAttachShader(this->program, fragmentShader);
	glLinkProgram(this->program);
	GLint shaderLogSize;
	glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s", buf);
		delete[] buf;
	}
}

void ShaderObject::makeUniform(std::string name)
{
	int location = glGetUniformLocation(program, name.c_str());
	uniformLocation.insert(std::pair<std::string, int>(name, location));
}
void ShaderObject::uploadUniformMatrix4(std::string name, Matrix4 mat4)
{
	glUniformMatrix4fv(uniformLocation[name], 1, GL_TRUE, &mat4[0]);
}
void ShaderObject::uploadUniformVector4(std::string name, Vec4 vec4) 
{
	glUniform4fv(uniformLocation[name], 1, &vec4.x);
}

void ShaderObject::useProgram()
{
	glUseProgram(program);
}

void ShaderObject::quitProgram()
{
	glUseProgram(0);
}

void ShaderObject::destroy()
{
	glDeleteProgram(program);
}
