#include "pch.h"

using namespace std;
using namespace DirectX::SimpleMath;

UIElement::UIElement() :
    m_relativePosition(Vector2::Zero),
    m_size(Vector2::Zero),
    m_parent(nullptr),
    m_active(true),
    m_children(std::list<std::shared_ptr<UIElement> >())
{
}

UIElement::~UIElement()
{
    DeleteChildren();
    m_parent.reset();
}

void UIElement::DeleteChildren()
{
    for (auto& child : m_children)
    {
        child->DeleteChildren();
        child->m_parent.reset();
        child.reset();
    }

    m_children.clear();
}

void UIElement::AddChild(shared_ptr<UIElement> child)
{
    shared_ptr<UIElement> prevParent = child->m_parent;

    if (prevParent != nullptr)
        prevParent->m_children.remove_if([&](const shared_ptr<UIElement> uiElement)
            {
                return child.get() == uiElement.get();
            });

    child->m_parent = shared_from_this();
    m_children.push_back(child);
}

void UIElement::Update(shared_ptr<UICamera> uiCamera)
{
    if (!m_active)
        return;

    ElementUpdate(uiCamera);

    for (auto& child : m_children)
        child->Update(uiCamera);
}

void UIElement::Render(shared_ptr<UICamera> uiCamera)
{
    if (!m_active)
        return;

    ElementRender(uiCamera);

    for (auto& child : m_children)
        child->Render(uiCamera);
}

Vector2 UIElement::GetAbsolutePosition()
{
    Vector2 result = m_relativePosition;
    if (m_parent != nullptr)
        result += m_parent->GetAbsolutePosition();

    return result;
}
