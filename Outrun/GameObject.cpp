#include "pch.h"
#include "GameObject.h"

using namespace std;

GameObject::GameObject()
{
    m_gameComponents = list<shared_ptr<GameComponent> >();
}

GameObject::GameObject(const GameObject& other)
{
}

GameObject::~GameObject()
{
    m_gameComponents.clear();
}

void GameObject::Update(float deltaTime)
{
    for (shared_ptr<GameComponent>& gc : m_gameComponents)
        gc->Update(deltaTime);
}

void GameObject::Render(Pseudo3DCamera* pseudo3dCamera)
{
    for (shared_ptr<GameComponent>& gc : m_gameComponents)
        gc->Render(pseudo3dCamera);
}