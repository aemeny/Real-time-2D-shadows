#pragma once
#include "Shader.h"

struct Core;

struct QuadRenderer
{
	QuadRenderer(std::string _vertShaderPath, std::string _fragShaderPath, const char* _voronoiTexPath, const char* _randomNoiseTexture);
	~QuadRenderer();
	void renderQuad(); // render a quad based on GUI location an texture
	
	// Setters
	void setHeightMapTexture(std::shared_ptr<Texture> _texture) { m_heightMapTexture = _texture; }

	std::shared_ptr<Shader> m_shader;
private:
	std::shared_ptr<Texture> m_heightMapTexture;
	std::shared_ptr<Texture> m_voronoiTexture;
	std::shared_ptr<Texture> m_randomNoiseTexture;
	std::shared_ptr<Vao> m_quad;
};

