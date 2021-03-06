#include "pch.h"
#include "Player.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Player::Player(GameObject* parent) :

    GameComponent(parent),
    m_spriteRenderer(nullptr),
    m_positionX(0.0f),
    m_speed(0.0f),
    m_steerSpeed(0.0f),
    m_distance(0.0f),
    m_livesCount(0)

{
}

Player::~Player()
{
    m_spriteRenderer.reset();
}

void Player::Update(float deltaTime)
{
    InputManager*              inputManager = InputManager::GetInstance();
    Game*                      game         = m_parent->GetGame();
    shared_ptr<Pseudo3DCamera> camera       = game->GetPseudo3DCamera();
    shared_ptr<Transform>      transform    = m_parent->GetTransform();

    transform->SetPositionZ(camera->GetZ(camera->GetHeight() - 1));

    if (inputManager->GetKeyDown(InputManager::GameKey::Up))
        m_speed += SPEED_ACCELERATION_MULTIP * deltaTime;
    else
        m_speed -= SPEED_ACCELERATION_MULTIP * deltaTime;

    if (inputManager->GetKeyDown(InputManager::GameKey::Down))
        m_speed -= BRAKE_MULTIP * deltaTime;

    if (m_speed >= MAX_SPEED)
        m_speed = MAX_SPEED;

    if (m_speed < 0.0f)
        m_speed = 0.0f;

    bool steering = false;
      
    if (inputManager->GetKeyDown(InputManager::GameKey::Left))
    {
        m_steerSpeed -= deltaTime * STEER_ACCELERATION_MULTIP;
        steering = true;
    }
    if (inputManager->GetKeyDown(InputManager::GameKey::Right))
    {
        m_steerSpeed += deltaTime * STEER_ACCELERATION_MULTIP;
        steering = true;
    }

    if (m_steerSpeed >= MAX_STEER_SPEED)
        m_steerSpeed = MAX_STEER_SPEED;
    if (m_steerSpeed <= -MAX_STEER_SPEED)
        m_steerSpeed = -MAX_STEER_SPEED;

    if (!steering)
    {
        if (m_steerSpeed > 0.0f)
            m_steerSpeed -= deltaTime * STEER_ACCELERATION_DECREASE;
        else
            m_steerSpeed += deltaTime * STEER_ACCELERATION_DECREASE;
    }

    Vector2 speedVector = Vector2(m_steerSpeed, m_speed);
    speedVector.Normalize();

    speedVector *= abs(m_speed);

    m_positionX += speedVector.x * deltaTime;

    game->GetTerrain()->SetPlayerSpeed(speedVector.y);

    if (abs(-game->GetTerrain()->GetAccumulatedTranslation() + m_positionX) <= game->GetRoadWidth())
        m_distance += speedVector.y * deltaTime;

    transform->SetPositionX(m_positionX - game->GetTerrain()->GetAccumulatedTranslation());

    camera->TranslateStripes(speedVector.y * deltaTime);
    camera->SetPositionX(m_positionX);
}

void Player::OnCollisionUpdate(shared_ptr<GameObject> other)
{
    if (other->GetComponent<Killer>() != nullptr)
        (*this)--;
    else if (other->GetComponent<Life>() != nullptr)
        (*this)++;
    else
        m_speed -= ACCIDENT_BRAKE;

    other->Die();
}

Player& Player::operator++()
{
    operator+=(1);

    return *this;
}

Player& Player::operator--()
{
    m_livesCount--;

    if (m_livesCount < 0)
        m_parent->Die();

    return *this;
}

Player& Player::operator++(int)
{
    operator++();
    return *this;
}

Player& Player::operator--(int)
{
    operator--();
    return *this;
}

Player& Player::operator+=(const int& rhs)
{
    m_livesCount += rhs;

    if (m_livesCount >= MAX_LIVES_COUNT)
        m_livesCount = MAX_LIVES_COUNT;

    return *this;
}
