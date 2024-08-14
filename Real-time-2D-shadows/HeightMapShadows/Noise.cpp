#include "Noise.h"

Noise::Noise(std::weak_ptr<Core> _core, int _texWidth, int _texHeight) : 
    m_core(_core), m_texWidth(_texWidth), m_texHeight(_texHeight), m_currentThread(-1),
    m_seed(0), m_noiseFrequency(0.0144f), m_waterLevel(0.3f)
{
    int length = m_texHeight * m_texWidth * 4;
    for (size_t i = 0; i < length; i++)
    {
        m_heightMap.push_back(0);
    }
}

Noise::~Noise() {}

void Noise::generatePerlinTexture()
{
    m_mutex.lock();
    m_currentThread++;

    int yStart = m_currentThread * ((m_texHeight) / m_numOfThreads);
    int yEnd = (m_currentThread + 1) * ((m_texHeight) / m_numOfThreads);
    if (m_currentThread == m_numOfThreads - 1)
        yEnd = m_texHeight;

    int index = yStart * m_texWidth * 4;
    int length = m_texHeight * m_texWidth * 4;
    //std::cout << "Index:" << index << " Size:" << m_heightMap.size() << " Truelength:" << length << std::endl;
    m_mutex.unlock();

    for (int y = yStart; y < yEnd; y++)
    {
        for (int x = 0; x < m_texWidth; x++)
        {
            float n = noiseValue(x, y);
            n *= 4.0f;

            m_heightMap[index] = (glm::clamp(n - 3.0f, 0.0f, 1.0f));
            index++;
            m_heightMap[index] = (glm::clamp(n - 2.0f, 0.0f, 1.0f));
            index++;
            m_heightMap[index] = (glm::clamp(n - 1.0f, 0.0f, 1.0f));
            index++;
            m_heightMap[index] = (glm::clamp(n, 0.0f, 1.0f));
            index++;
        }
    }
}

float Noise::noiseValue(float _x, float _y)
{
    float n = glm::perlin(glm::vec3(_x * m_noiseFrequency, _y * m_noiseFrequency, m_seed * 2)) * 0.5f + 0.5f;
    
    n *= islandMod(_x, _y);

    return glm::max(m_waterLevel, n);
}

float Noise::islandMod(float _x, float _y)
{
    float maxD = glm::min(m_texWidth, m_texHeight);
    maxD = (maxD * 0.5f) * (maxD * 0.5f);

    float dx = m_texWidth * 0.5f - _x;
    float dy = m_texHeight * 0.5f - _y;

    float dsq = (dx * dx) + (dy * dy);
    
    return map(dsq, 0.0f, maxD, 1.25f, 0.0f);
}

float Noise::map(float _n, float _start1, float _stop1, float _start2, float _stop2)
{
    return ((_n - _start1) / (_stop1 - _start1)) * (_stop2 - _start2) + _start2;
};
