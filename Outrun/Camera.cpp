#include "pch.h"
#include "Camera.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(ID3D11Device*        d3dDevice,
	           ID3D11DeviceContext* d3dContext,
	  		   int                  width,
			   int                  height,
			   int                  screenWidth,
			   int                  screenHeight) :

	m_rendering2D(false),
	m_d3dContext(d3dContext),
	m_renderTexture(make_unique<RenderTexture>(d3dDevice, 
		                                       width, 
		                                       height)),
	m_spriteBatch(make_unique<SpriteBatch>(d3dContext)),
	m_width(width),
	m_height(height),
	m_screenWidth(screenWidth),
	m_screenHeight(screenHeight),
	m_states(make_unique<CommonStates>(d3dDevice))
{
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
	m_states.reset();
	m_spriteBatch.reset();
	m_renderTexture.reset();
}

void Camera::Present(SpriteBatch* spriteBatch)
{
	if (!spriteBatch)
		return;

	float scale = GetPresentScale();

	float targetWidth  = m_width  * scale;
	float targetHeight = m_height * scale;

	Vector2 screenCenter = Vector2(m_screenWidth, 
		                           m_screenHeight) * 0.5f;

	Vector2 position = Vector2(screenCenter.x - (targetWidth / 2.0f), 
		                       screenCenter.y - (targetHeight / 2.0f));

	spriteBatch->Draw(m_renderTexture->GetShaderResourceView(), 
		              position, 
		              nullptr, 
		              Colors::White, 
		              0.0f, 
		              Vector2::Zero, 
		              scale, 
		              SpriteEffects::SpriteEffects_None, 
		              0.0f);
}

void Camera::DrawSprite(Texture2D*  sprite,
	                    Vector2     position, 
	                    const RECT* sourceRectangle = nullptr, 
	                    float       rotation        = 0.0f, 
	                    Vector2     scale           = Vector2::One)
{
	Begin2D();

	Vector2 centerPosition = Vector2::Zero;
	Vector2 zoom           = Vector2::One;
	Vector2 textureScale   = Vector2::One;

	RECT rect   = RECT();
	rect.left   = 0;
	rect.top    = 0;
	rect.bottom = sprite->GetWidth();
	rect.right  = sprite->GetHeight();

	if (!sourceRectangle)
		sourceRectangle = &rect;

	textureScale = scale;

	Vector2 centerTranslation = Vector2(sourceRectangle->right  / 2.0f,
		                                sourceRectangle->bottom / 2.0f);

	centerTranslation *= textureScale;

	Vector2 calculatedPosition = (Vector2((int)position.x, (int)position.y) * zoom) -
		                         (centerTranslation * zoom);

	calculatedPosition -= Vector2(((int)centerPosition.x * zoom.x) - (m_width / 2.0f),
		                          ((int)centerPosition.y * zoom.y) - (m_height / 2.0f));

	Vector2 calculatedScale = zoom * textureScale;

	// This is for camera clamping..
	// like, to not show objects that are not inside the camera
	// I mention this here because I can't firgure it out if you don't show me this comment.
	RECT offsettedScreenRect   = RECT();

	offsettedScreenRect.left   =            (int)(-sourceRectangle->right  * calculatedScale.x);
	offsettedScreenRect.top    =            (int)(-sourceRectangle->bottom * calculatedScale.y);
	offsettedScreenRect.right  = m_width  + (int)( sourceRectangle->right  * calculatedScale.x);
	offsettedScreenRect.bottom = m_height + (int)( sourceRectangle->bottom * calculatedScale.y);

	if (calculatedPosition.x                                                 >= offsettedScreenRect.left  &&
		calculatedPosition.x + (sourceRectangle->right * calculatedScale.x)  <= offsettedScreenRect.right &&
		calculatedPosition.y                                                 >= offsettedScreenRect.top   &&
		calculatedPosition.y + (sourceRectangle->bottom * calculatedScale.y) <= offsettedScreenRect.bottom)

		m_spriteBatch->Draw(sprite->GetShaderResourceView(), 
			                calculatedPosition, 
			                sourceRectangle, 
			                Colors::White, 
			                rotation, 
			                Vector2::Zero, 
			                calculatedScale, 
			                SpriteEffects::SpriteEffects_None, 
			                0.0f);
}

void Camera::Begin(Vector4 clearColor)
{
	m_renderTexture->SetRenderTarget(m_d3dContext);
	m_renderTexture->ClearRenderTarget(m_d3dContext,
                                       clearColor.x,
                                       clearColor.y,
                                       clearColor.z,
                                       clearColor.w);


}

void Camera::End(ID3D11RenderTargetView* const* renderTargetViews, 
	             ID3D11DepthStencilView*        depthStencilView, 
	             int                            numViews)
{
	End2D();
	m_d3dContext->OMSetRenderTargets(numViews,
		                             renderTargetViews,
		                             depthStencilView);
}

void Camera::Begin2D()
{
	if (!m_rendering2D)
	{
		m_spriteBatch->Begin(DirectX::SpriteSortMode_Deferred,
			                 nullptr,
			                 m_states->PointClamp());

		m_rendering2D = true;
	}
}

void Camera::End2D()
{
	if (m_rendering2D)
	{
		m_spriteBatch->End();
		m_rendering2D = false;
	}
}

float Camera::GetPresentScale()
{
	float screenAspectRatio = (float)m_screenWidth / (float)m_screenHeight;
	float cameraAspectRatio = (float)m_width       / (float)m_height;
	float scale;

	if (screenAspectRatio > cameraAspectRatio)
		scale = (float)m_screenHeight / (float)m_height;
	else
		scale = (float)m_screenWidth / (float)m_width;

	// so that all the pixels are the same size
	// if I don't do this, the image will be scaled to match the window
	// width or height, but the pixels won't be the same size
	scale = (int)scale;

	return scale;
}
