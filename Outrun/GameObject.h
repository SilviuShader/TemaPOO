#pragma once

class Game;

class GameObject
{
public:
    
    GameObject(Game*);
    GameObject(const GameObject&);
    ~GameObject();

           void  Update(float);
           void  Render(Pseudo3DCamera*);

    template <class T> std::shared_ptr<GameComponent> GetComponent();

    inline             void                           AddComponent(std::shared_ptr<GameComponent> gameComponent) { m_gameComponents.push_back(gameComponent); }
    
    inline             std::shared_ptr<Transform>     GetTransform() const                                       { return m_transform;                        }
    inline             Game*                          GetGame()      const                                       { return m_game;                             }

private:

    std::list<std::shared_ptr<GameComponent> > m_gameComponents;
    Game*                                      m_game;
    std::shared_ptr<Transform>                 m_transform;
};

template<class T>
inline std::shared_ptr<GameComponent> GameObject::GetComponent()
{
    for (auto& component : m_gameComponents)
        if (dynamic_cast<T*>(component.get()) != NULL)
            return component;

    return nullptr;
}
