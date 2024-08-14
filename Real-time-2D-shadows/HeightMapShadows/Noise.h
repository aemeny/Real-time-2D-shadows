#pragma once
#include <glm/gtc/noise.hpp>
#include <GL/glew.h>
#include <iostream>
#include <mutex>
#include "Texture.h"

struct Core;

struct Noise
{
    Noise(std::weak_ptr<Core> _core, int _texWidth, int _texHeight);
    ~Noise();

    void generatePerlinTexture();
    std::vector<float> getHeightMap() { return m_heightMap; }
    void setSeed(float _seed) { m_seed = _seed; }
    void setnumOfThreads(int _numOfThreads) { m_numOfThreads = _numOfThreads; }
    void setCurrentThread(int _currentThread) { m_currentThread = _currentThread; }
private:
    float map(float _n, float _start1, float _stop1, float _start2, float _stop2);
    float islandMod(float _x, float _y);
    float noiseValue(float _x, float _y);
    
    std::weak_ptr<Core> m_core;
    std::vector<float> m_heightMap;

    std::mutex m_mutex; // Prevents data races in noise class

    int m_texHeight;
    int m_texWidth;

    int m_currentThread;
    int m_numOfThreads;

    float m_noiseFrequency;
    float m_seed;
    float m_waterLevel;
};

