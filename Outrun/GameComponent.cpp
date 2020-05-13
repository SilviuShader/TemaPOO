#include "pch.h"
#include "GameComponent.h"

using namespace std;

GameComponent::GameComponent(GameObject* parent) : 
    m_parent(parent)
{
}

GameComponent::~GameComponent()
{
    m_parent = nullptr;
}
