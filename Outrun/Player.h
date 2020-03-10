#pragma once

class Player : public GameComponent
{
private:

    const float STEER_ACCELERATION_MULTIP = 1.0f;
    const float SPEED_ACCELERATION_MULTIP = 1.0f;
    const float SENSE_OF_SPEED            = 2.5f;

public:

    Player(GameObject*);
    ~Player();

    void Update(float)           override;
    void Render(Pseudo3DCamera*) override;

    inline float GetPositionX()  const { return m_positionX; }

private:

    std::shared_ptr<SpriteRenderer> m_spriteRenderer;
    float                           m_positionX;
    float                           m_posXVelocity;
    float                           m_speed;
    float                           m_accumultedStripesTranslation;
};