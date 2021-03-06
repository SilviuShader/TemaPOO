#include "pch.h"

using namespace std;
using namespace DirectX::SimpleMath;

UIButton::UIButton(Vector2                 position, 
                   Vector2                 size, 
                   shared_ptr<Texture2D>   activeTexture, 
                   shared_ptr<Texture2D>   pressedTexture,
                   function<void _cdecl()> onReleasedFunction) :
    
    m_activeTexture(activeTexture),
    m_pressedTexture(pressedTexture),
    m_onReleasedFunction(onReleasedFunction),
    m_pressed(false),
    m_selected(false)

{
    SetRelativePosition(position);
    SetSize(size);
    
    if (m_pressedTexture == nullptr)
        m_pressedTexture = m_activeTexture;
}

UIButton::~UIButton()
{
    m_pressedTexture.reset();
    m_activeTexture.reset();
}

void UIButton::ElementUpdate(shared_ptr<UICamera> uiCamera)
{
    Vector2 absolutePosition = GetAbsolutePosition();
    Vector2 mousePosition    = uiCamera->GetWorldPosition(uiCamera->GetCameraPosition(InputManager::GetInstance()->GetMousePosition()));

    bool isMouseDown     = InputManager::GetInstance()->GetMouseButtonDown(InputManager::MouseButton::Left);
    bool isMouseReleased = InputManager::GetInstance()->GetMouseButtonReleased(InputManager::MouseButton::Left);

    if (m_selected && isMouseReleased)
        m_onReleasedFunction();

    m_pressed = false;

    if (absolutePosition.x - (m_size.x / 2.0f) <= mousePosition.x &&
        absolutePosition.x + (m_size.x / 2.0f) >= mousePosition.x &&
        absolutePosition.y - (m_size.y / 2.0f) <= mousePosition.y &&
        absolutePosition.y + (m_size.y / 2.0f) >= mousePosition.y)
    {
        m_selected = true;
        if (isMouseDown)
            m_pressed = true;
    }
    else
        m_selected = false;
}

void UIButton::ElementRender(shared_ptr<UICamera> uiCamera)
{
    Vector2 absolutePosition = GetAbsolutePosition();
    shared_ptr<Texture2D> textureToDraw = m_activeTexture;

    if (m_pressed)
        textureToDraw = m_pressedTexture;

    uiCamera->DrawSprite(textureToDraw,
                         absolutePosition, 
                         nullptr, 
                         0.0f,
                         Vector2(m_size.x / (float)textureToDraw->GetWidth(), 
                                 m_size.y / (float)textureToDraw->GetHeight()));
}
