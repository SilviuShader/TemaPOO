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

	        void Begin2D(std::function<void _cdecl()> = nullptr);
	        void End2D();

	inline  int GetWidth()  { return m_width; }
	inline  int GetHeight() { return m_height; }

private:

	float GetPresentScale();
	float Clamp(float, float, float);

protected:


	ID3D11DeviceContext* m_d3dContext;

	int                  m_width;
	int                  m_height;

private:

	bool                                   m_rendering2D;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::function<void _cdecl()>           m_customBeginFunction;

	std::unique_ptr<RenderTexture>         m_renderTexture;
	std::unique_ptr<DirectX::SpriteBatch>  m_spriteBatch;

	int                                    m_screenWidth;
	int                                    m_screenHeight;
};
