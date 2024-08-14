#include "Shader.h"
#include <iostream>
#include <fstream>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

/* Constructor for the shader taking in a vertex and fragment shader file location */
Shader::Shader(std::string _vertFileName, std::string _fragFileName)
{
	m_programId = glCreateProgram();
	//calling create shader on each fragment and vertex shader giving it the correct type
	m_vertShader = createShader(_vertFileName, GL_VERTEX_SHADER);
	m_fragShader = createShader(_fragFileName, GL_FRAGMENT_SHADER);
	//linking attribute locations
	link();
	
	m_animate = false;
	m_waterLevel = 0.35f;
	m_heightMod = 0.01;
}

/* shader deconstructor */
Shader::~Shader()
{
	// Detach and destroy the shader objects. These are no longer needed
	// because we now have a complete shader program.
	glDetachShader(m_vertShader, m_programId);
	glDetachShader(m_fragShader, m_programId);
	glDeleteShader(m_vertShader);
	glDeleteShader(m_fragShader);
}

void Shader::renderQuad(std::shared_ptr<Vao> _Vao, std::shared_ptr<Texture> _heightTexture, std::shared_ptr<Texture> _voronoiTexture, std::shared_ptr<Texture> _randomNoiseTexture)
{
	// Instruct OpenGL to use our shader program and our VAO
	glUseProgram(m_programId);

	//glBindVertexArray(vaoId);
	glBindVertexArray(_Vao->getId());

	//Set active texure
	glActiveTexture(GL_TEXTURE0);

	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, _heightTexture->getId());
	glUniform1i(glGetUniformLocation(m_programId, "height"), 0);

	//Set active texure
	glActiveTexture(GL_TEXTURE1);

	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, _voronoiTexture->getId());
	glUniform1i(glGetUniformLocation(m_programId, "voronoiTex"), 1);
	
	//Set active texure
	glActiveTexture(GL_TEXTURE2);

	//Bind Texture
	glBindTexture(GL_TEXTURE_2D, _randomNoiseTexture->getId());
	glUniform1i(glGetUniformLocation(m_programId, "randomNoise"), 2);

	//Depth Testing
	glEnable(GL_DEPTH_TEST);

	//Alpha Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw 3 vertices (a triangle)
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Reset the state
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/* link attribute locations and link to program */
void Shader::link()
{
	// Ensure the VAO "position"  
	glBindAttribLocation(m_programId, 0, "a_Position");
	glBindAttribLocation(m_programId, 1, "a_TexCoord");

	// Perform the link and check for failure
	GLint success = 1;
	glLinkProgram(m_programId);
	glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		throw std::exception();
	}
}

void Shader::setShaderValues(glm::vec2 _sunPos, int _windowWidth, int _windowHeight, float _DT)
{
	glUseProgram(m_programId);
	GLint linkStatus;
	glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		std::cerr << "Error linking shader program: " << std::endl;
	}

	GLuint sunPosLocation = glGetUniformLocation(m_programId, "sunPos");
	if (sunPosLocation == -1) { std::cerr << "Could not find uniform location for sunPos" << std::endl;}
	GLuint pixLocation = glGetUniformLocation(m_programId, "pix");
	if (pixLocation == -1) { std::cerr << "Could not find uniform location for pix" << std::endl;}
	GLint millisLocation = glGetUniformLocation(m_programId, "millis");
	if (millisLocation == -1) { std::cerr << "Could not find uniform location for millis" << std::endl;}
	GLint heightModLocation = glGetUniformLocation(m_programId, "heightMod");
	if (heightModLocation == -1) { std::cerr << "Could not find uniform location for heightMod" << std::endl;}
	GLint waterLevelLocation = glGetUniformLocation(m_programId, "waterLevel");
	if (waterLevelLocation == -1) {	std::cerr << "Could not find uniform location for waterLevel" << std::endl;}
	GLint resLocation = glGetUniformLocation(m_programId, "res");
	if (resLocation == -1) { std::cerr << "Could not find uniform location for res" << std::endl;}
	GLint rotLocation = glGetUniformLocation(m_programId, "rot");
	if (rotLocation == -1) { std::cerr << "Could not find uniform location for rot" << std::endl; }

	glm::vec3 sunPosition(_sunPos, 1.0f);
	glUniform3fv(sunPosLocation, 1, glm::value_ptr(sunPosition));

	glm::vec3 pix(1.0f / _windowWidth, 1.0f / _windowHeight, 0.0f);
	glUniform3fv(pixLocation, 1, glm::value_ptr(pix));

	glUniform1f(millisLocation, (GLfloat)SDL_GetTicks());
	
	glUniform1f(waterLevelLocation, m_waterLevel);

	if (_DT < 0.02f)
	{
		if (m_animate)
		{
			m_heightMod = lerp(m_heightMod, 1.05f, _DT);
			if (m_heightMod >= 1.0f)
			{
				m_heightMod = 1.0f;
				m_animate = false;
			}
		}
	}
	glUniform1f(heightModLocation, m_heightMod);

	glUniform2fv(resLocation, 1, glm::value_ptr(glm::vec2(_windowWidth / _windowHeight, 1.0f)));

	glUniform2fv(rotLocation, 1, glm::value_ptr(glm::mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50))));

	glUseProgram(0);
}

float Shader::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

bool Shader::shrinkIsland(float _DT)
{
	if (_DT < 0.02f)
	{
		m_heightMod = lerp(m_heightMod, -0.03f, _DT * 0.5f);
		if (m_heightMod <= 0.01f)
		{
			m_heightMod = 0.01f;
			return true;
		}
	}
	return false;
}

/* bind matrix given which matrix and a type */
void Shader::bindShader(glm::mat4 _matrix, const char* _type)
{
	//Bind matrix
	glUseProgram(getId());
	glProgramUniformMatrix4fv(m_programId, glGetUniformLocation(m_programId, _type), 1, GL_FALSE, glm::value_ptr(_matrix));
	glUseProgram(0);
}

/* using fstream to open and read the given shader files and convert them into a string to be given to open gl */
std::string Shader::readFile(std::string _fileName)
{
	std::string line;
	std::string fileString = "";
	std::ifstream file(_fileName);
	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			fileString += line + '\n';
		}
		file.close();
	}
	else throw std::runtime_error("Unable To Open Shader File");

	return fileString;
}

/* creating a shader given the file location and type */
GLuint Shader::createShader(std::string _fileName, GLuint _type)
{
	//open file and return its insides as a string
	std::string fileString = readFile(_fileName);
	const GLchar* ShaderSrc = fileString.c_str();

	// Create a new shader, attach source code, compile it and
	// check for errors.
	GLuint shaderId = glCreateShader(_type);
	glShaderSource(shaderId, 1, &ShaderSrc, NULL);
	glCompileShader(shaderId);

	GLint success = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) //Print error in exact location for easier debugging
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH,
			&maxLength);
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shaderId, maxLength,
			&maxLength, &errorLog[0]);
		std::cout << &errorLog.at(0) << std::endl;
		throw std::exception();
	}

	//Attach Shader
	glAttachShader(m_programId, shaderId);

	return shaderId;
}