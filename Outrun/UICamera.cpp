#include "pch.h"

using namespace std;
using namespace Microsoft::WRL;

using namespace DirectX;
using namespace DirectX::SimpleMath;

UICamera::UICamera(ComPtr<ID3D11Device>        d3dDevice, 
                   ComPtr<ID3D11DeviceContext> d3dContext, 
                   Game*                       game, 
                   int                         width, 
                   int                         height, 
                   int                         screenWidth, 
                   int                         screenHeight) :
    Camera(d3dDevice, 
           d3dContext, 
           game, 
           width, 
           height, 
           screenWidth, 
           screenHeight)
{
}

UICamera::~UICamera()
{
}

void UICamera::DrawString(shared_ptr<GameFont> font, 
                          string               message, 
                          Vector2              position)
{
    if (font == nullptr)
        return;


    Vector2 zoom              = Vector2::One;
    Vector2 centerPosition    = Vector2::Zero;
    Vector2 messageSize       = font->GetSpriteFont()->MeasureString(message.c_str());
    
    Vector2 centerTranslation = Vector2(messageSize.x / 2.0f,
                                        messageSize.y / 2.0f);

    Vector2 calculatedPosition = (Vector2((int)position.x, (int)position.y) * zoom) -
                                 (centerTranslation * zoom);

    calculatedPosition        -= Vector2(((float)centerPosition.x * zoom.x) - (m_width / 2.0f),
                                         ((float)centerPosition.y * zoom.y) - (m_height / 2.0f));

    calculatedPosition         = Vector2((int)calculatedPosition.x,
                                         (int)calculatedPosition.y);

    Begin2D(m_customBeginFunction);

    font->GetSpriteFont()->DrawString(m_spriteBatch.get(), 
                                      message.c_str(), 
                                      calculatedPosition, 
                                      font->GetColor(), 
                                      0.0f, 
                                      Vector2::Zero, 
                                      zoom, 
                                      SpriteEffects_None, 
                                      1.0f);
}

Vector2 UICamera::GetCameraPosition(Vector2 screenPosition)
{
    float scale           = GetPresentScale();

    float targetWidth     = m_width  * scale;
    float targetHeight    = m_height * scale;

    Vector2 screenCenter  = Vector2(m_screenWidth, 
                                    m_screenHeight) * 0.5f;
    Vector2 position      = Vector2(screenCenter.x - (targetWidth  / 2.0f), 
                                    screenCenter.y - (targetHeight / 2.0f));
    
    Vector2 imagePosition = Vector2((int)position.x, 
                                    (int)position.y);

    Vector2 posInRect     = Vector2(screenPosition.x, 
                                    screenPosition.y) - 
                            Vector2(imagePosition.x, 
                                    imagePosition.y);
    
    posInRect /= scale;

    return posInRect;
}

Vector2 UICamera::GetWorldPosition(Vector2 cameraPosition)
{
    Vector2 centerPosition = Vector2::Zero;
    Vector2 zoom           = Vector2::One;
    
    Vector2 position = cameraPosition - Vector2(m_width / 2.0f,
                                                m_height / 2.0f);
    
    position += centerPosition * zoom;

    return position;
}
