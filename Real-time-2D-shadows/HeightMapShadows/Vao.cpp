#include "Vao.h"
#include <stdexcept>

Vao::Vao() : m_id(0), m_dirty(false)
{}

/* if id isnt made create the vertext array */
GLuint Vao::getId()
{
	if (!m_id)
	{
		glGenVertexArrays(1, &m_id);

		if (!m_id)
			throw std::runtime_error("Fail to allocate Vao");
	}
	if (m_dirty)
	{
		m_dirty = false;

		glBindVertexArray(m_id);
		//Loop through all given vbos and set their id and components in the buffer and the attrib pointer
		for (int i = 0; i < m_vbos.size(); i++)
		{
			if (m_vbos[i]->getComponents() != 0)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_vbos[i]->getId());
				glVertexAttribPointer(i, m_vbos[i]->getComponents(), GL_FLOAT, GL_FALSE, 0, (void*)0);
				glEnableVertexAttribArray(i);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
		glBindVertexArray(0);
	}

	return m_id;
}

/* add passed vbos to the list of vbos */
void Vao::addVbo(std::shared_ptr<Vbo> _vbo)
{
	m_vbos.push_back(_vbo);
	_vbo->setVao(this);
	if (_vbo->getComponents())
		m_dirty = true;
}