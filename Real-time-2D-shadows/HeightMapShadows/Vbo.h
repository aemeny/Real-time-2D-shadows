#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

struct Vao;

struct Vbo
{
	Vbo();
	void add(const glm::vec3& _value);
	void add(const glm::vec2& _value);
	void clearData();
	GLuint getId();
	int getComponents() { return m_components; }
	void setVao(Vao* _vao) { m_vao = _vao; }
	int getIdentifierID() { return vboIdentifierID; }

private:
	GLuint m_id;
	Vao* m_vao;
	std::vector<float> m_data;
	bool m_dirty;
	int m_components;
	int vboIdentifierID;
};

