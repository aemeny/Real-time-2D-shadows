#include "Vbo.h"
#include "Vao.h"
#include <stdexcept>

Vbo::Vbo() : m_id(0), m_dirty(false), m_components(0), m_vao(nullptr)
{}

/* using polymorphism add given values to m_data list and set how many components */
void Vbo::add(const glm::vec3& _value)
{
	m_data.push_back(_value.x);
	m_data.push_back(_value.y);
	m_data.push_back(_value.z);
	m_dirty = true;
	m_components = 3;
	if (m_vao != nullptr)
		m_vao->setDirty(true);
}

void Vbo::add(const glm::vec2& _value)
{
	m_data.push_back(_value.x);
	m_data.push_back(_value.y);
	m_dirty = true;
	m_components = 2;
	if (m_vao != nullptr)
		m_vao->setDirty(true);
}

void Vbo::clearData()
{
	m_data.clear();
	m_dirty = false;
	m_components = 0;
	if (m_vao != nullptr)
		m_vao->setDirty(false);
}

/* if id isnt created create a new id and bind the data */
GLuint Vbo::getId()
{
	if (!m_id)
	{
		glGenBuffers(1, &m_id);
		if (!m_id)
			throw std::runtime_error("Failed to allocate VBO");
	}

	if (m_dirty)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
		// Upload a copy of the data from memory into the new VBO
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_data.size(), &m_data.at(0), GL_STATIC_DRAW);
		// Reset the state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_dirty = false;
	}

	return m_id;
}