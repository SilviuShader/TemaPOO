#pragma once

class RenderTexture
{
public:

	RenderTexture(ID3D11Device*, 
		          int, 
		          int);

	RenderTexture(const RenderTexture&);

	~RenderTexture();

	void                      SetRenderTarget(ID3D11DeviceContext*);
	void                      ClearRenderTarget(ID3D11DeviceContext*, 
		                                        float, 
		                                        float, 
		                                        float, 
		                                        float);

	ID3D11ShaderResourceView* GetShaderResourceView();

	int                       GetTextureWidth();
	int                       GetTextureHeight();

private:

	void Initialize(ID3D11Device*, int, int);
	void Shutdown();

private:

	int                       m_textureWidth, 
		                      m_textureHeight;

	ID3D11Texture2D*          m_renderTargetTexture;
	ID3D11RenderTargetView*   m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
	ID3D11Texture2D*          m_depthStencilBuffer;
	ID3D11DepthStencilView*   m_depthStencilView;
	D3D11_VIEWPORT            m_viewport;
};