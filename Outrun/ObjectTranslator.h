#pragma once

class ObjectTranslator : public GameComponent
{
public:

    ObjectTranslator(GameObject*);
    ~ObjectTranslator();

           void Update(float)                                  override;
           void Render()                                       override  {                        }
    inline void SetObjectSpeed(float speed)                              { m_objectSpeed = speed; }

private:

    float m_objectSpeed;
};