#include "pch.h"
#include "GameComponent.h"

using namespace std;

GameComponent::GameComponent(shared_ptr<GameObject> parent) : 
    m_parent(parent)
{
}

GameComponent::~GameComponent()
{
    m_parent.reset();
}
