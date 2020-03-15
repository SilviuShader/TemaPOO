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
        Right
    };

public:
    
    InputManager(std::shared_ptr<DirectX::Keyboard>, 
                 std::shared_ptr<DirectX::Mouse>);
    ~InputManager();

           static void          CreateInstance(std::shared_ptr<DirectX::Keyboard>, 
                                               std::shared_ptr<DirectX::Mouse>);
           static void          Reset();
    inline static InputManager* GetInstance() { return g_inputManager; }

public:

    // I will explicitly name the enum
    // for "extra" safety... like in C#
    bool                         GetKey(InputManager::GameKey);
    DirectX::SimpleMath::Vector2 GetMousePosition();

private:

    static InputManager* g_inputManager;

private:

    std::shared_ptr<DirectX::Keyboard> m_keyboard;
    std::shared_ptr<DirectX::Mouse>    m_mouse;
};