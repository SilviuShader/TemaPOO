#include "pch.h"
#include "Player.h"

using namespace std;
using namespace DirectX;

Player::Player(GameObject* parent) :

    GameComponent(parent),
    m_spriteRenderer(nullptr),
    m_positionX(0.0f),
    m_posXVelocity(0.0f),
    m_speed(0.0f),
    m_accumultedStripesTranslation(0.0f)
{
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
    InputManager*              inputManager = InputManager::GetInstance();
    Game*                      game         = m_parent->GetGame();
    Pseudo3DCamera*            camera       = game->GetPseudo3DCamera();
    std::shared_ptr<Transform> transform    = m_parent->GetTransform();

    transform->SetLine(camera->GetHeight() - 1);

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
        m_positionX -= deltaTime;
    }
    if (inputManager->GetKey(InputManager::GameKey::Right))
    {
        m_positionX += deltaTime;
    }

    m_positionX += m_posXVelocity * deltaTime * m_speed;
    m_accumultedStripesTranslation += m_speed * deltaTime;
    m_parent->GetGame()->GetTerrain()->SetPlayerSpeed(m_speed);

    transform->SetPositionX(m_positionX - game->GetTerrain()->GetAccumulatedTranslation(camera->GetHeight() - 1));
}

void Player::Render(Pseudo3DCamera* pseudo3DCamera)
{
    pseudo3DCamera->TranslateStripes(m_accumultedStripesTranslation);
    m_accumultedStripesTranslation = 0.0f;

    pseudo3DCamera->SetPositionX(m_positionX);
}
