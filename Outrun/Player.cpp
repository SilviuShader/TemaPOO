#include "pch.h"
#include "Player.h"

using namespace std;
using namespace DirectX;

Player::Player(shared_ptr<GameObject> parent) :

    GameComponent(parent),
    m_spriteRenderer(nullptr),
    m_positionX(0.0f),
    m_speed(0.0f)

{
}

Player::~Player()
{
    m_spriteRenderer.reset();
}

void Player::Update(float deltaTime)
{
    InputManager*              inputManager = InputManager::GetInstance();
    shared_ptr<Game>           game         = m_parent->GetGame();
    shared_ptr<Pseudo3DCamera> camera       = game->GetPseudo3DCamera();
    std::shared_ptr<Transform> transform    = m_parent->GetTransform();

    transform->SetPositionZ(camera->GetZ(camera->GetHeight() - 1));

    if (inputManager->GetKey(InputManager::GameKey::Up))
        m_speed += SPEED_ACCELERATION_MULTIP * deltaTime;
    else
        m_speed -= SPEED_ACCELERATION_MULTIP * deltaTime;

    if (m_speed >= MAX_SPEED)
        m_speed = MAX_SPEED;

    if (m_speed < 0.0f)
        m_speed = 0.0f;

      
    if (inputManager->GetKey(InputManager::GameKey::Left))
        m_positionX -= deltaTime * STEER_ACCELERATION_MULTIP;
    if (inputManager->GetKey(InputManager::GameKey::Right))
        m_positionX += deltaTime * STEER_ACCELERATION_MULTIP;

    m_parent->GetGame()->GetTerrain()->SetPlayerSpeed(m_speed);

    transform->SetPositionX(m_positionX - game->GetTerrain()->GetAccumulatedTranslation());

    camera->TranslateStripes(m_speed * deltaTime);
    camera->SetPositionX(m_positionX);
}

void Player::OnCollisionUpdate(shared_ptr<GameObject> other)
{
    m_parent->Die();
}
