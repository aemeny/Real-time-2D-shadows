#pragma once
#include <string>
#include "Vao.h"
#include "Texture.h"

struct Shader
{
	Shader(std::string _vertFileName, std::string _fragFileName);
	~Shader();
	void renderQuad(std::shared_ptr<Vao> _Vao, std::shared_ptr<Texture> _heightTexture, std::shared_ptr<Texture> _voronoiTexture, std::shared_ptr<Texture> _randomNoiseTexture);
	
	void bindShader(glm::mat4 _matrix, const GLchar* _type);
	const GLuint getId() { return m_programId; };

	void setShaderValues(glm::vec2 _sunPos, int _windowWidth, int _windowHeight, float _DT);
	void setheightMod(float _height) { m_heightMod = _height; }
	void setanimate(bool _animate) { m_animate = _animate; }

	bool shrinkIsland(float _DT);

private:
	float lerp(float a, float b, float f);
	void link();
	GLuint createShader(std::string _fileName, GLuint _type);
	std::string readFile(std::string _fileName);

	std::string m_fileName;
	GLuint m_programId;
	GLuint m_fragShader;
	GLuint m_vertShader;

	float m_heightMod;
	float m_waterLevel;
	bool m_animate;
};

