#include "pch.h"
#include "GameObject.h"

using namespace std;

GameObject::GameObject(shared_ptr<Game> game) :
    m_game(game),
    m_dead(false),
    m_transform(nullptr)
{
    m_gameComponents = list<shared_ptr<GameComponent> >();
}

GameObject::~GameObject()
{
    m_transform.reset();
    m_game.reset();
    ClearComponents();
}

void GameObject::Update(float deltaTime)
{
    for (shared_ptr<GameComponent>& gc : m_gameComponents)
        gc->Update(deltaTime);
}

void GameObject::Render()
{
    for (shared_ptr<GameComponent>& gc : m_gameComponents)
        gc->Render();
}

std::shared_ptr<Transform> GameObject::GetTransform()
{
    // Every objects needs a transform,
    // unfortunately it can't be created in the constructor
    // because the the constructor of transform needs a shared_ptr to
    // the game object.
    if (m_transform == nullptr)
    {
        m_transform = std::make_shared<Transform>(shared_from_this());
        m_gameComponents.push_back(m_transform);
    }

    return m_transform;
}

void GameObject::ClearComponents()
{
    for (auto& gc : m_gameComponents)
        gc.reset();

    m_gameComponents.clear();
}
