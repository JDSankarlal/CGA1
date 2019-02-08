#include "ShaderProgram.h"
#include "IO.h"
#include <fstream>
#include "Camera.h"

bool ShaderProgram::_IsInitDefault = false;

std::string ShaderProgram::_ShaderDirectory = "../assets/shaders/";
GLuint ShaderProgram::_VertShaderDefault = 0;
GLuint ShaderProgram::_FragShaderDefault = 0;
GLuint ShaderProgram::_ProgramDefault = 0;

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(const std::string & vertFile, const std::string & fragFile)
{
	load(vertFile, fragFile);
}

ShaderProgram::~ShaderProgram()
{
	if (_IsInit)
	{
		unload();
	}
}

bool ShaderProgram::initDefault()
{
	if (!_IsInitDefault)
	{
		ShaderProgram errorShader; 
		bool compileSuccess = errorShader.load("error.vert", "error.frag");
		if (!compileSuccess)
		{
			SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Could not compile default Shaders!");
			system("PAUSE");
			exit(EXIT_FAILURE);
		}
		ShaderProgram::_VertShaderDefault = errorShader._VertShader;
		ShaderProgram::_FragShaderDefault   = errorShader._FragShader;
		ShaderProgram::_ProgramDefault		= errorShader._Program;
		_IsInitDefault = true;
	}
	return _IsInitDefault;
}

void ShaderProgram::setDefault()
{
	_VertShader = _VertShaderDefault;
	_FragShader = _FragShaderDefault;
	_Program = _ProgramDefault;
}

bool ShaderProgram::load(const std::string & vertFile, const std::string & fragFile)
{
	_VertFilename = vertFile;
	_FragFilename = fragFile;

	// Create shader and program objects
	_VertShader = glCreateShader(GL_VERTEX_SHADER);
	_FragShader = glCreateShader(GL_FRAGMENT_SHADER);
	_Program = glCreateProgram();


	// Load our source code for shaders
	std::string source = readFile(_ShaderDirectory + vertFile);
	const GLchar *temp = static_cast<const GLchar *>(source.c_str());
	glShaderSource(_VertShader, 1, &temp, NULL);

	source = readFile(_ShaderDirectory + fragFile);
	temp = static_cast<const GLchar *>(source.c_str());
	glShaderSource(_FragShader, 1, &temp, NULL);

	// Compile vertex and frag shaders
	if (!compileShader(_VertShader))
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Vertex Shader failed to compile.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", vertFile.c_str());
		outputShaderLog(_VertShader);
		unload();
		setDefault();
		return false;
	}

	if (!compileShader(_FragShader))
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Fragment Shader failed to compile.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", fragFile.c_str());
		outputShaderLog(_FragShader);
		unload();
		setDefault();
		return false;
	}

	glAttachShader(_Program, _VertShader);
	glAttachShader(_Program, _FragShader);

	if (!linkProgram())
	{
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] Shader Program failed to link.");
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", vertFile.c_str());
		SAT_DEBUG_LOG_ERROR("[ShaderProgram.cpp] %s", fragFile.c_str());
		outputProgramLog();
		unload();
		setDefault();
		return false;
	}

	_IsInit = true;
	return true;
}

bool ShaderProgram::reload()
{
	return load(_VertFilename, _FragFilename);
}

bool ShaderProgram::isLoaded() const
{
	return _IsInit;
}

void ShaderProgram::unload()
{
	if (_VertShader != 0 && _VertShader != _VertShaderDefault) //If the Vertex shader exists, remove it.
	{
		glDetachShader(_Program, _VertShader);
		glDeleteShader(_VertShader);
		_VertShader = 0;
	}
	if (_FragShader != 0 && _FragShader != _FragShaderDefault)
	{
		glDetachShader(_Program, _FragShader);
		glDeleteShader(_FragShader);
		_FragShader = 0;
	}
	if (_Program != 0 && _Program != _ProgramDefault) //If the Fragment shader exists, remove it.
	{
		glDeleteProgram(_Program);
		_Program = 0;
	}
	_IsInit = false;
}

bool ShaderProgram::linkProgram()
{
	glLinkProgram(_Program);
	GLint success;
	glGetProgramiv(_Program, GL_LINK_STATUS, &success);
	return success == GL_TRUE;
}

void ShaderProgram::bind() const
{
	glUseProgram(_Program);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

void ShaderProgram::bindUBO(const std::string & uniformBlockName, unsigned int bindSlot) const
{
	GLuint index = glGetUniformBlockIndex(_Program, uniformBlockName.c_str());
	glUniformBlockBinding(_Program, index, bindSlot);
}

GLint ShaderProgram::getUniformLocation(const std::string & uniformName) const
{
	GLint uniformLoc = glGetUniformLocation(_Program, uniformName.c_str());	
#if _DEBUG
	if (uniformLoc == -1)
	{
		SAT_DEBUG_LOG_WARNING("[ShaderProgram.cpp]  Uniform %s not found!", uniformName.c_str());
	}
#endif
	return uniformLoc;
}

void ShaderProgram::sendUniform(const std::string & name, const float scalar) const
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, scalar);
}

void ShaderProgram::sendUniform(const std::string & name, const vec3 & vector) const
{
	GLint location = getUniformLocation(name);
	glUniform3fv(location, 1, &vector.x);
}

void ShaderProgram::sendUniform(const std::string & name, const vec4 & vector) const
{
	GLint location = getUniformLocation(name);
	glUniform4fv(location, 1, &vector.x);
}

void ShaderProgram::sendUniform(const std::string & name, const mat4 & matrix, bool transpose) const
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, transpose, matrix.data);
}

void ShaderProgram::sendUniformCamera(Camera * camera)
{
	sendUniform("uView", camera->getView());
	sendUniform("uProj", camera->getProjection());
}

bool ShaderProgram::compileShader(GLuint shader) const
{
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success == GL_TRUE;
}

void ShaderProgram::outputShaderLog(GLuint shader) const
{
	std::vector<char> infoLog;
	infoLog.resize(512);

	GLint infoLen;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	glGetShaderInfoLog(shader, sizeof(char) * 512, &infoLen, &infoLog[0]); //Size of char array in bits for portability, rather than characters.
	SAT_DEBUG_LOG_WARNING("%s", std::string(infoLog.begin(), infoLog.end()).c_str());
}

void ShaderProgram::outputProgramLog() const
{
	std::vector<char> infoLog;
	infoLog.resize(512);

	GLint infoLen;
	glGetProgramiv(_Program, GL_INFO_LOG_LENGTH, &infoLen);
	glGetProgramInfoLog(_Program, sizeof(char) * 512, &infoLen, &infoLog[0]); // Size of char array in bits for portability, rather than characters.
	SAT_DEBUG_LOG_WARNING("%s", std::string(infoLog.begin(), infoLog.end()).c_str());
}
