#include "pch.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

UIText::UIText(shared_ptr<GameFont> font, 
               Vector2              position) :

    m_font(font)

{
    SetRelativePosition(position);
    m_size = Vector2::Zero;
}

UIText::~UIText()
{
    m_font.reset();
}

void UIText::SetText(string message)
{
    m_message = message;
    m_size = m_font->GetSpriteFont()->MeasureString(message.c_str());
}

void UIText::ElementRender(shared_ptr<UICamera> uiCamera)
{
    Vector2 absolutePosition = GetAbsolutePosition();
    uiCamera->DrawString(m_font, 
                         m_message, 
                         absolutePosition);
}
