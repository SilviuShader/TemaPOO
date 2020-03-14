#pragma once

class ObjectTranslator : public GameComponent
{
public:

    ObjectTranslator(std::shared_ptr<GameObject>);
    ~ObjectTranslator();

           void Update(float)                                  override;
    inline void Render()                                       override  {                        };
           void OnCollisionUpdate(std::shared_ptr<GameObject>) override;

    inline void SetObjectSpeed(float speed)                              { m_objectSpeed = speed; }

private:

    float m_objectSpeed;
};