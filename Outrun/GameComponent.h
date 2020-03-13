#pragma once

class GameObject;
class Camera;
class Pseudo3DCamera;

class GameComponent
{
public:

    GameComponent(std::shared_ptr<GameObject>);
    ~GameComponent();

    virtual void                        Update(float) = 0;
    virtual void                        Render()      = 0;

    inline  std::shared_ptr<GameObject> GetParent() const { return m_parent; }

protected:

    std::shared_ptr<GameObject> m_parent;
};