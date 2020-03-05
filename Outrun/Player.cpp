#include "pch.h"
#include "Player.h"

using namespace std;
using namespace DirectX;

Player::Player(shared_ptr<Terrain> terrain) :
    m_terrain(terrain),
    m_positionX(0.0f),
    m_posXVelocity(0.0f),
    m_speed(0.0f),
    m_accumultedStripesTranslation(0.0f)
{
}

Player::Player(const Player& other)
{
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
    InputManager* inputManager = InputManager::GetInstance();

    if (inputManager->GetKey(InputManager::GameKey::Up))
        m_speed += SPEED_ACCELERATION_MULTIP * deltaTime;
    else
        m_speed -= SPEED_ACCELERATION_MULTIP * deltaTime;

    if (m_speed >= 3.0f)
        m_speed = 3.0f;
    if (m_speed < 0.0f)
        m_speed = 0.0f;

    bool steer = false;
      
    if (inputManager->GetKey(InputManager::GameKey::Left))
    {
        m_posXVelocity -= STEER_ACCELERATION_MULTIP * deltaTime;
        steer = true;
    }
    if (inputManager->GetKey(InputManager::GameKey::Right))
    {
        m_posXVelocity += STEER_ACCELERATION_MULTIP * deltaTime;
        steer = true;
    }
    if (abs(m_posXVelocity) >= 1.0f)
        m_posXVelocity = m_positionX / abs(m_posXVelocity);

    if (!steer && m_positionX != 0.0f)
        m_posXVelocity -= (m_positionX / abs(m_posXVelocity)) * STEER_ACCELERATION_MULTIP * deltaTime;

    m_positionX += m_posXVelocity * deltaTime * m_speed;
    m_accumultedStripesTranslation += m_speed * deltaTime * SENSE_OF_SPEED;
    m_terrain->SetPlayerSpeed(m_speed);
}

void Player::Render(Pseudo3DCamera* pseudo3DCamera)
{
    pseudo3DCamera->TranslateStripes(m_accumultedStripesTranslation);
    m_accumultedStripesTranslation = 0.0f;

    pseudo3DCamera->SetPositionX(m_positionX);
}
