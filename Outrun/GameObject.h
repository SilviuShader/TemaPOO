#pragma once

class Game;

class GameObject
{
public:
    
    GameObject(Game*);
    ~GameObject();

           void                           Update(float);
           void                           Render();
           void                           OnCollisionUpdate(std::shared_ptr<GameObject>);

           std::shared_ptr<Transform>     GetTransform();

           void                           ClearComponents();


    template <class T> 
           std::shared_ptr<GameComponent> GetComponent() const;

    // This could be called suicide too because the object itself does the action
    inline void                           Die()                                                      { m_dead = true;                             }
    inline void                           AddComponent(std::shared_ptr<GameComponent> gameComponent) { m_gameComponents.push_back(gameComponent); }
    inline Game*                          GetGame() const                                            { return m_game;                             }
    inline bool                           Dead()    const                                            { return m_dead;                             }

private:

    std::list<std::shared_ptr<GameComponent> > m_gameComponents;
    Game*                                      m_game;
    std::shared_ptr<Transform>                 m_transform;
    bool                                       m_dead;
};

template<class T>
inline std::shared_ptr<GameComponent> GameObject::GetComponent() const
{
    for (auto& component : m_gameComponents)
        if (dynamic_cast<T*>(component.get()) != nullptr)
            return component;

    return nullptr;
}
