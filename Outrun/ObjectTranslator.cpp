#include "pch.h"
#include "ObjectTranslator.h"

using namespace std;

ObjectTranslator::ObjectTranslator(shared_ptr<GameObject> parent) :
    GameComponent(parent),
    m_objectSpeed(0.0f)
{
    shared_ptr<Transform> transform = m_parent->GetTransform();
    transform->SetPositionZ(m_parent->GetGame()->GetPseudo3DCamera()->GetCameraDepth());
}

ObjectTranslator::~ObjectTranslator()
{
}

void ObjectTranslator::Update(float deltaTime)
{
    shared_ptr<Player>    player    = m_parent->GetGame()->GetPlayer();
    float                 speed     = player->GetSpeed();
    
    shared_ptr<Transform> transform = m_parent->GetTransform();

    float                 translate = (speed + m_objectSpeed) * deltaTime;

    transform->SetPositionZ(transform->GetPositionZ() - translate);

    // the child kills his parent
    // what he doesn't know is that he will die too.
    if (transform->GetPositionZ() <= m_parent->GetGame()->GetObjectDisappearDepth() ||
        transform->GetPositionZ() >= (m_parent->GetGame()->GetPseudo3DCamera()->GetCameraDepth() * 2.0f))
        m_parent->Die();
}

void ObjectTranslator::OnCollisionUpdate(shared_ptr<GameObject> other)
{
    // This object hit something
    // Or it was hit by something
    // NOW IT MUST DIE.
    m_parent->Die();
}
