#pragma once

class GameObject
{
public:
    
    GameObject();
    GameObject(const GameObject&);
    ~GameObject();

           void Update(float);
           void Render(Pseudo3DCamera*);

    inline void AddComponent(std::shared_ptr<GameComponent> gameComponent) { m_gameComponents.push_back(gameComponent); }

private:

    std::list<std::shared_ptr<GameComponent> > m_gameComponents;
};