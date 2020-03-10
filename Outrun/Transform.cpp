#include "pch.h"
#include "Transform.h"

Transform::Transform(GameObject* parent) :
    GameComponent(parent),
    m_screenLine(0),
    m_positionX(0),
    m_scale(1.0f)
{
}

Transform::Transform(GameObject* parent, 
                     int         screenLine, 
                     float       positionX) :
    GameComponent(parent),
    m_screenLine(screenLine),
    m_positionX(positionX),
    m_scale(1.0f)
{
}

Transform::~Transform()
{
}

void Transform::Update(float deltaTime)
{
    Game*           game   = m_parent->GetGame();
    Pseudo3DCamera* camera = game->GetPseudo3DCamera();
    float z = camera->GetZ(m_screenLine);
    m_scale = 1.0f / z;
}