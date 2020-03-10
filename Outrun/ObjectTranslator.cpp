#include "pch.h"
#include "ObjectTranslator.h"

using namespace std;

ObjectTranslator::ObjectTranslator(GameObject* parent) :
    GameComponent(parent),
    m_translation(0.0f)
{
    shared_ptr<Transform> transform = m_parent->GetTransform();
    transform->SetLine(m_parent->GetGame()->GetPseudo3DCamera()->GetLine(15.0f));
}

ObjectTranslator::~ObjectTranslator()
{
}

void ObjectTranslator::Update(float deltaTime)
{
    Player* player = m_parent->GetGame()->GetPlayer();
    float   speed  = player->GetSpeed();
    
    shared_ptr<Transform> transform = m_parent->GetTransform();

    m_translation += speed * deltaTime;
    transform->SetLine(m_parent->GetGame()->GetPseudo3DCamera()->GetLine(15.0f - m_translation));

    // the child kills his parent
    // what he doesn't know is that he will die too.
    if (m_translation >= 20.0f)
        m_parent->Die();
}

void ObjectTranslator::Render(Pseudo3DCamera* camera)
{
}
