#pragma once

class Player : public GameComponent
{
private:

    const float STEER_ACCELERATION_MULTIP = 1.0f;
    const float SPEED_ACCELERATION_MULTIP = 1.0f;
    const float SENSE_OF_SPEED            = 2.5f;

public:

    Player(std::shared_ptr<DirectX::Keyboard>, 
           std::shared_ptr<Terrain>);
    Player(const Player&);
    ~Player();

    void Update(float)           override;
    void Render(Pseudo3DCamera*) override;

private:

    std::shared_ptr<DirectX::Keyboard> m_keyboard;
    std::shared_ptr<Terrain>           m_terrain;
    float                              m_positionX;
    float                              m_posXVelocity;
    float                              m_speed;
    float                              m_accumultedStripesTranslation;
};