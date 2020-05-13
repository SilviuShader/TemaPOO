#pragma once

class GameObject;
class Camera;
class Pseudo3DCamera;

class GameComponent
{
public:

             GameComponent(GameObject*);
    virtual ~GameComponent();

    virtual void        Update(float) = 0;
    virtual void        Render()      = 0;

    // this is not a function that is required for many objects
    // that's why it doesn't need override most of the time.
    virtual void        OnCollisionUpdate(std::shared_ptr<GameObject>) {                  }

    inline  GameObject* GetParent() const                              { return m_parent; }

protected:

    GameObject* m_parent;
};