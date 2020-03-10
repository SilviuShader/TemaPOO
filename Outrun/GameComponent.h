#pragma once

class GameObject;
class Camera;
class Pseudo3DCamera;

class GameComponent
{
public:

    GameComponent(GameObject*);
    GameComponent(const GameComponent&);
    ~GameComponent();

    virtual void       Update(float)           = 0;
    virtual void       Render(Pseudo3DCamera*) = 0;

    inline GameObject* GetParent() const { return m_parent; }

protected:

    GameObject* m_parent;
};