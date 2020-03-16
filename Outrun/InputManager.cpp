#include "pch.h"
#include "InputManager.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

std::shared_ptr<InputManager> InputManager::g_inputManager = nullptr;

InputManager::InputManager(shared_ptr<Keyboard> keyboard,
                           shared_ptr<Mouse>    mouse) :
    m_keyboard(keyboard),
    m_mouse(mouse),
    m_lastMousePosition(Vector2(-1, -1)),
    m_mouseMoved(false)
{
    for (int i = 0; i < (int)InputManager::GameKey::Last; i++)
    {
        m_keyboardStates[i] = false;
        m_getKeys       [i] = false;
    }

    for (int i = 0; i < (int)InputManager::MouseButton::Last; i++)
    {
        m_mouseStates           [i] = false;
        m_getMouseButtons       [i] = false;
        m_getMouseButtonsRelease[i] = false;
    }
}

InputManager::~InputManager()
{
    m_mouse.reset();
    m_keyboard.reset();
}

void InputManager::CreateInstance(shared_ptr<Keyboard> keyboard, 
                                  shared_ptr<Mouse>    mouse)
{
    if (g_inputManager != nullptr)
        Reset();

    g_inputManager = make_shared<InputManager>(keyboard, 
                                               mouse);
}

void InputManager::Reset()
{
    if (g_inputManager != nullptr)
    {
        g_inputManager.reset();
        g_inputManager = nullptr;
    }
}

void InputManager::Update()
{
    // Initialize values
    for (int i = 0; i < (int)InputManager::GameKey::Last; i++)
        m_getKeys[i] = false;

    for (int i = 0; i < (int)InputManager::MouseButton::Last; i++)
    {
        m_getMouseButtons       [i] = false;
        m_getMouseButtonsRelease[i] = false;
    }
    m_mouseMoved = false;

    // Update values
    for (int i = 0; i < (int)InputManager::GameKey::Last; i++)
        if (GetKeyDown((InputManager::GameKey)i) && !m_keyboardStates[i])
            m_getKeys[i] = true;

    for (int i = 0; i < (int)InputManager::GameKey::Last; i++)
        m_keyboardStates[i] = GetKeyDown((InputManager::GameKey)i);

    for (int i = 0; i < (int)InputManager::MouseButton::Last; i++)
    {
        if (GetMouseButtonDown((InputManager::MouseButton)i) && !m_mouseStates[i])
            m_getMouseButtons[i] = true;

        if (!GetMouseButtonDown((InputManager::MouseButton)i) && m_mouseStates[i])
            m_getMouseButtonsRelease[i] = true;
    }

    for (int i = 0; i < (int)InputManager::MouseButton::Last; i++)
        m_mouseStates[i] = GetMouseButtonDown((MouseButton)i);

    Vector2 actualMousePos = GetMousePosition();
    if ((int)actualMousePos.x != (int)m_lastMousePosition.x ||
        (int)actualMousePos.y != (int)m_lastMousePosition.y)
    {
        m_lastMousePosition = actualMousePos;
        m_mouseMoved = true;
    }
}

bool InputManager::GetKeyDown(InputManager::GameKey key)
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
    case InputManager::GameKey::Esc:
        result = keyboardState.Escape;
        break;
    }

    return result;
}

bool InputManager::GetMouseButtonDown(InputManager::MouseButton button)
{
    Mouse::State mouseState = m_mouse->GetState();
    bool         result     = false;

    switch (button)
    {
    case InputManager::MouseButton::Left:
        result = mouseState.leftButton;
        break;
    case InputManager::MouseButton::Middle:
        result = mouseState.middleButton;
        break;
    case InputManager::MouseButton::Right:
        result = mouseState.rightButton;
        break;
    }

    return result;
}
