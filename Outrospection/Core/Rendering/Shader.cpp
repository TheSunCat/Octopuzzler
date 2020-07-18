#include "Shader.h"
#include "Core.h"

#include <fstream>
#include <sstream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Util.h"
#include "Core/Camera.h"

// TODO make this class... nicer
Shader::Shader(const GLchar* vertexName, const GLchar* fragmentName)
{
	std::string vName(vertexName);
	std::string veName(("res/ShaderData/" + vName + ".vert"));
	const GLchar* vertexPath = veName.c_str();
	std::string fName(fragmentName);
	std::string frName("res/ShaderData/" + fName + ".frag");
	const GLchar* fragmentPath = (frName).c_str();

	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		LOG_ERROR("Failed to read shader file \"%s\" or \"%s\"! errno %s", vertexPath, fragmentPath, e.what());
		return;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();


	// 2. compile shaders
	GLuint vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		LOG_ERROR("Vertex shader %s failed to compile, error log:\n%s", vertexPath, infoLog);
	}

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		LOG_ERROR("Fragment shader %s failed to compile, error log:\n%s", fragmentPath, infoLog);
	}

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		LOG_ERROR("Failed to link shader programs %s and %s, error log:\n%s", vertexPath, fragmentPath, infoLog);
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::doProjView(Camera& _camera, const int _width, const int _height, const bool doPos) const
{
	const glm::mat4 projection = glm::perspective(glm::radians(_camera.zoom), float(_width) / float(_height), 0.1f, 100.0f);

	glm::mat4 view;
	if (doPos)
		view = _camera.getViewMatrix();
	else
		view = glm::lookAt(glm::vec3(0.0), _camera.front, _camera.up);

	setMat4("projection", projection);
	setMat4("view", view);
}

// use/activate the shader
void Shader::use() const {
	glUseProgram(ID);
}

// utility uniform functions
void Shader::setBool(const std::string& name, const bool value) const {
	glUniform1i(getUniformLocation(name.c_str()), int(value));
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, const int value) const {
	glUniform1i(getUniformLocation(name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, const float value) const {
	glUniform1f(getUniformLocation(name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(getUniformLocation(name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, const float x, const float y) const
{
	glUniform2f(getUniformLocation(name.c_str()), x, y);
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(getUniformLocation(name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const
{
	glUniform3f(getUniformLocation(name.c_str()), x, y, z);
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4fv(getUniformLocation(name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, const float x, const float y, const float z, const float w) const
{
	glUniform4f(getUniformLocation(name.c_str()), x, y, z, w);
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
	glUniformMatrix2fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
	glUniformMatrix3fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(getUniformLocation(name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

GLint Shader::getUniformLocation(const char* uniformName) const
{
	std::size_t hash = Util::hashBytes(uniformName, strlen(uniformName));

	const auto f = uniformCache.find(hash);

	GLint loc;

	if (f == uniformCache.end()) {// get uniform location
		loc = glGetUniformLocation(ID, uniformName);

		std::pair<std::size_t, GLuint> newLoc(hash, loc);
		uniformCache.insert(newLoc);
	}
	else
	{
		loc = (*f).second;
	}

	return loc;
}

// utility function for checking shader compilation/linking errors
void Shader::checkCompileErrors(const GLuint shader, const std::string& type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			LOG_ERROR("Shader compilation error! Type: %s\n%s", type, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			LOG_ERROR("Shader linking error! Type: %s\n%s", type, infoLog);
		}
	}
}