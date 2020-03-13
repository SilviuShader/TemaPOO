#pragma once

class Player : public GameComponent
{
private:

    const float MAX_SPEED                 = 3.0f;
    const float STEER_ACCELERATION_MULTIP = 1.0f;
    const float SPEED_ACCELERATION_MULTIP = 1.0f;

public:

    Player(std::shared_ptr<GameObject>);
    ~Player();

           void Update(float)    override;
    inline void Render()         override {                     }

    inline float GetPositionX()  const    { return m_positionX; }
    inline float GetSpeed()      const    { return m_speed;     }

private:

    std::shared_ptr<SpriteRenderer> m_spriteRenderer;
    float                           m_positionX;
    float                           m_speed;
};