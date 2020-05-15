#pragma once

// I usually avoid using singletons
// but I'm supposed to use as many design patterns
// as I can..
class InputManager
{
public:

    // I hope it's ok to delcare
    // a public enum inside a class.
    enum class GameKey
    {
        Up,
        Left,
        Down,
        Right,
        Esc,
        Last
    };

    enum class MouseButton
    {
        Left,
        Middle,
        Right,
        Last
    };

public:
    
    
    static void          CreateInstance(std::shared_ptr<DirectX::Keyboard>,
                                        std::shared_ptr<DirectX::Mouse>);
    static void          DeleteInstance();
    static InputManager* GetInstance() { return g_inputManager; }

private:

    InputManager(std::shared_ptr<DirectX::Keyboard>,
                 std::shared_ptr<DirectX::Mouse>);
    ~InputManager();

    InputManager(const InputManager&)           = delete;
    InputManager operator=(const InputManager&) = delete;

public:

           void                         Update();

           bool                         GetKeyDown(InputManager::GameKey);
           bool                         GetMouseButtonDown(InputManager::MouseButton);
           
    inline bool                         GetKey(InputManager::GameKey key)                        const { return m_getKeys               [(int)key   ];               }
    inline bool                         GetMouseButton(InputManager::MouseButton button)         const { return m_getMouseButtons       [(int)button];               }
    inline bool                         GetMouseButtonReleased(InputManager::MouseButton button) const { return m_getMouseButtonsRelease[(int)button];               }
    inline bool                         MouseMoved()                                             const { return m_mouseMoved;                                        }
    inline DirectX::SimpleMath::Vector2 GetMousePosition()                                       const { return DirectX::SimpleMath::Vector2(m_mouse->GetState().x, 
                                                                                                                                             m_mouse->GetState().y); }

private:

    static InputManager* g_inputManager;

private:

    std::shared_ptr<DirectX::Keyboard> m_keyboard;
    std::shared_ptr<DirectX::Mouse>    m_mouse;

    bool                               m_keyboardStates        [(int)InputManager::GameKey    ::Last];
    bool                               m_getKeys               [(int)InputManager::GameKey    ::Last];

    bool                               m_mouseStates           [(int)InputManager::MouseButton::Last];
    bool                               m_getMouseButtons       [(int)InputManager::MouseButton::Last];
    bool                               m_getMouseButtonsRelease[(int)InputManager::MouseButton::Last];
    DirectX::SimpleMath::Vector2       m_lastMousePosition;
    bool                               m_mouseMoved;
};