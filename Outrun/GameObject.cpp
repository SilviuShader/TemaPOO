#include "pch.h"
#include "GameObject.h"

using namespace std;

GameObject::GameObject(Game* game) :
    m_game(game)
{
    m_gameComponents = list<shared_ptr<GameComponent> >();
    // Every game object needs to have a transform component
    // Like in Unity...
    m_transform = make_shared<Transform>(this);
    m_gameComponents.push_back(m_transform);
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