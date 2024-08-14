#include "QuadRenderer.h"
#include "Core.h"

QuadRenderer::~QuadRenderer()
{}
/*
* initialize function called from component, creats a quad (two triangles) and attaches the orthographic camera
*/
QuadRenderer::QuadRenderer(std::string _vertShaderPath, std::string _fragShaderPath, const char* _voronoiTexPath, const char* _randomNoiseTexture)
{
	std::shared_ptr<Vbo> pos = std::make_shared<Vbo>();
	pos->add(glm::vec3(-1.0f, 1.0f, 0.0f));
	pos->add(glm::vec3(-1.0f, -1.0f, 0.0f));
	pos->add(glm::vec3(1.0f, -1.0f, 0.0f));

	pos->add(glm::vec3(1.0f, -1.0f, 0.0f));
	pos->add(glm::vec3(1.0f, 1.0f, 0.0f));
	pos->add(glm::vec3(-1.0f, 1.0f, 0.0f));

	std::shared_ptr<Vbo> textCoords = std::make_shared<Vbo>();
	textCoords->add(glm::vec2(0.0f, 1.0f));
	textCoords->add(glm::vec2(0.0f, 0.0f));
	textCoords->add(glm::vec2(1.0f, 0.0f));
	textCoords->add(glm::vec2(1.0f, 0.0f));
	textCoords->add(glm::vec2(1.0f, 1.0f));
	textCoords->add(glm::vec2(0.0f, 1.0f));

	m_quad = std::make_shared<Vao>();
	m_quad->addVbo(pos);
	m_quad->addVbo(textCoords);

	m_shader = std::make_shared<Shader>(_vertShaderPath, _fragShaderPath);
	m_voronoiTexture = std::make_shared<Texture>(_voronoiTexPath);
	m_randomNoiseTexture = std::make_shared<Texture>(_randomNoiseTexture);
}

/*
* renderes a quad based on the texture and size given
*/
void QuadRenderer::renderQuad()
{
	m_shader->renderQuad(m_quad, m_heightMapTexture, m_voronoiTexture, m_randomNoiseTexture);
}