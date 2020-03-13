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
    
    InputManager(std::shared_ptr<DirectX::Keyboard>);
    ~InputManager();

           static void          CreateInstance(std::shared_ptr<DirectX::Keyboard>);
           static void          Reset();
    inline static InputManager* GetInstance() { return g_inputManager; }

public:

    // I will explicitly name the enum
    // for "extra" safety... like in C#
    bool GetKey(InputManager::GameKey);

private:

    static InputManager* g_inputManager;

private:

    std::shared_ptr<DirectX::Keyboard> m_keyboard;
};