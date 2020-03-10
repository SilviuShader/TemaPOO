#include "pch.h"
#include "GameComponent.h"

GameComponent::GameComponent(GameObject* parent) : 
    m_parent(parent)
{
}

GameComponent::GameComponent(const GameComponent& other)
{
}

GameComponent::~GameComponent()
{
}
