#pragma once

class Player : public GameComponent
{
private:

    const float STEER_ACCELERATION_MULTIP = 1.0f;
    const float SPEED_ACCELERATION_MULTIP = 1.0f;

public:

    Player(GameObject*);
    ~Player();

    void Update(float)           override;
    void Render(Pseudo3DCamera*) override;

    inline float GetPositionX()  const { return m_positionX; }
    inline float GetSpeed()      const { return m_speed;     }

private:

    std::shared_ptr<SpriteRenderer> m_spriteRenderer;
    float                           m_positionX;
    float                           m_posXVelocity;
    float                           m_speed;
    float                           m_accumultedStripesTranslation;
};