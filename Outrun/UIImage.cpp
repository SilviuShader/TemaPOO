#include "pch.h"

using namespace std;

using namespace DirectX::SimpleMath;

UIImage::UIImage(shared_ptr<Texture2D> texture, 
                 Vector2               position,
                 Vector2               size) :
    m_texture(texture),
    m_rotation(0.0f)
{
    SetRelativePosition(position);
    SetSize(size);
}

UIImage::~UIImage()
{
    m_texture.reset();
}

UIImage& UIImage::operator=(const float& rotation)
{
    SetRotation(rotation);
    return *this;
}

void UIImage::ElementRender(shared_ptr<UICamera> uiCamera)
{
    Vector2 absolutePosition = GetAbsolutePosition();

    uiCamera->DrawSprite(m_texture,
                         absolutePosition,
                         nullptr,
                         m_rotation,
                         Vector2(m_size.x / (float)m_texture->GetWidth(),
                                 m_size.y / (float)m_texture->GetHeight()));
}
