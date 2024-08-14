#pragma once
#include <vector>
#include <GL/glew.h>

struct Texture
{
	Texture(std::vector<float>* _data, int _width, int _height);
	Texture(const char* _fileName);
	~Texture();

	const GLuint getId() { return m_textureId; };

private:

	void createTexture();
	void createId();
	void uploadToGPU();

	GLuint m_textureId;
	int m_width;
	int m_height;
	bool m_fromFile;

	std::vector<float>* m_vecData;
	unsigned char* m_data;
};

