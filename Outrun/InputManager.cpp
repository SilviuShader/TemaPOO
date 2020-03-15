#include "pch.h"
#include "InputManager.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

InputManager* InputManager::g_inputManager = NULL;

InputManager::InputManager(shared_ptr<Keyboard> keyboard,
                           shared_ptr<Mouse>    mouse) :
    m_keyboard(keyboard),
    m_mouse(mouse)
{
}

InputManager::~InputManager()
{
    m_mouse.reset();
    m_keyboard.reset();
}

void InputManager::CreateInstance(shared_ptr<Keyboard> keyboard, 
                                  shared_ptr<Mouse>    mouse)
{
    if (g_inputManager)
        Reset();

    g_inputManager = new InputManager(keyboard, 
                                      mouse);
}

void InputManager::Reset()
{
    if (g_inputManager)
    {
        delete g_inputManager;
        g_inputManager = NULL;
    }
}

bool InputManager::GetKey(InputManager::GameKey key)
{
    Keyboard::State keyboardState = m_keyboard->GetState();
    bool result = false;

    switch (key)
    {
    case InputManager::GameKey::Up:
        result = keyboardState.W || keyboardState.Up;
        break;
    case InputManager::GameKey::Left:
        result = keyboardState.A || keyboardState.Left;
        break;
    case InputManager::GameKey::Down:
        result = keyboardState.S || keyboardState.Down;
        break;
    case InputManager::GameKey::Right:
        result = keyboardState.D || keyboardState.Right;
        break;
    }

    return result;
}

Vector2 InputManager::GetMousePosition()
{
    Mouse::State mouseState = m_mouse->GetState();
    
    return Vector2(mouseState.x, 
                   mouseState.y);
}
