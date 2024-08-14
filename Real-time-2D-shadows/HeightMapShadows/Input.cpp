#include "Input.h"
#include "Core.h"

Input::Input(std::weak_ptr<Core> _core) :
    m_keys(), m_pressedKeys(), m_releasedKeys(), m_mousePos(glm::ivec2(0)),
    m_core(_core), m_lockMouse(false), m_mouseDown(false), m_mouseDownClick(false)
{}

Input::~Input() {}

/*
* returns if a key is currently being held down
*/
bool Input::isKey(int _key)
{
    for (int key : m_keys)
    {
        if (_key == key) {
            return true;
        }
    }
    return false;
}

/*
* returns if a key has been pressed this frame
*/
bool Input::isKeyPressed(int _key)
{
    for (int key : m_pressedKeys)
    {
        if (_key == key) {
            return true;
        }
    }
    return false;
}

/*
* returns if a key has been released this frame
*/
bool Input::isKeyReleased(int _key)
{
    for (int key : m_releasedKeys)
    {
        if (_key == key) {
            return true;
        }
    }
    return false;
}
/*
* loops through all SDL poll events to check for mouse, keyboard and controller input
*/
void Input::tick()
{
    while (SDL_PollEvent(&m_event) != 0)
    {
        if (m_event.type == SDL_MOUSEMOTION) //check for any mouse movement
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            m_mousePos.x = (float)x / (float)m_core.lock()->m_nativeWindow->m_windowWidth;
            m_mousePos.y = (float)y / (float)m_core.lock()->m_nativeWindow->m_windowHeight;
        }
        else if (m_event.type == SDL_KEYDOWN) //check if any keys have been pressed down
        {
            m_pressedKeys.push_back(m_event.key.keysym.sym); //Add to pressed keys this frame

            //Check if key is already in list
            bool check = true;
            for (int key : m_keys)
            {
                if (m_event.key.keysym.sym == key) {
                    check = false;
                }
            }
            if (check)
            {
                m_keys.push_back(m_event.key.keysym.sym); //Add to held keys list
            }
        }
        else if (m_event.type == SDL_KEYUP || m_event.type == SDL_MOUSEBUTTONUP)
        {
            m_releasedKeys.push_back(m_event.key.keysym.sym); //Add to released keys this frame

            //Remove from held keys list
            std::vector<int>::iterator position = std::find(m_keys.begin(), m_keys.end(), m_event.key.keysym.sym); //find key in list
            if (position != m_keys.end()) // == myVector.end() means the element was not found
                m_keys.erase(position);
        }
        else if (m_event.type == SDL_MOUSEBUTTONDOWN) // Mouse input
        {
            m_mouseDown = true;
            m_mouseDownClick = true;
        }
        else if (m_event.type == SDL_MOUSEBUTTONUP)
        {
            m_mouseDown = false;
        }
    }
}

void Input::clear()
{
    //Only valid for one frame
    m_pressedKeys.clear();
    m_releasedKeys.clear();
    m_mouseDownClick = false;
}
