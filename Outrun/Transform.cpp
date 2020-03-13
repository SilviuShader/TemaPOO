#include "pch.h"
#include "Transform.h"

using namespace std;

Transform::Transform(shared_ptr<GameObject> parent) :
    GameComponent(parent),
    m_positionX(0.0f),
    m_positionZ(0.0f),
    m_scale(1.0f)
{
}

Transform::Transform(shared_ptr<GameObject> parent, 
                     float                  positionX, 
                     float                  positionZ) :
    GameComponent(parent),
    m_positionX(positionX),
    m_positionZ(positionZ),
    m_scale(1.0f)
{
}

Transform::~Transform()
{
}

void Transform::Update(float deltaTime)
{
    if (m_positionZ != 0.0f)
        m_scale = 1.0f / m_positionZ;
}