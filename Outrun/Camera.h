#pragma once

class Camera
{
public:
	
	Camera(ID3D11Device*,
		   ID3D11DeviceContext*,
		   int, 
		   int, 
		   int, 
		   int);

	Camera(const Camera&);
	~Camera();

	void         Present(DirectX::SpriteBatch*);

	void         DrawSprite(Texture2D*,
		                    DirectX::SimpleMath::Vector2,
		                    const RECT*,
		                    float,
		                    DirectX::SimpleMath::Vector2);

	void         Begin(DirectX::SimpleMath::Vector4) ;

	virtual void End(ID3D11RenderTargetView* const*,
		             ID3D11DepthStencilView*,
		             int);
protected:

	virtual void Begin2D();
	virtual void End2D();

private:

	float GetPresentScale();

protected:

	ID3D11DeviceContext*                   m_d3dContext;
	std::unique_ptr<DirectX::CommonStates> m_states;

	int                                    m_width;
	int                                    m_height;

	bool                                   m_rendering2D;

private:

	std::unique_ptr<RenderTexture>        m_renderTexture;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	int                                   m_screenWidth;
	int                                   m_screenHeight;
};