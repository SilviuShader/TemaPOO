// I took this entire class from this tutorial: http://rastertek.com/dx11tut22.html
// There might be slight adjustments made by me (mostly changes to match the coding style of the project).

#pragma once

class RenderTexture
{
public:

	RenderTexture(Microsoft::WRL::ComPtr<ID3D11Device>, 
		          int, 
		          int);

	~RenderTexture();

	       void                                             SetRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext>);
	       void                                             ClearRenderTarget(Microsoft::WRL::ComPtr<ID3D11DeviceContext>, 
		                                                                      float, 
		                                                                      float, 
		                                                                      float, 
		                                                                      float);

	inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return m_shaderResourceView; }
	inline int                                              GetTextureWidth()       const { return m_textureWidth;       }
	inline int                                              GetTextureHeight()      const { return m_textureHeight;      }

private:

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>, 
		            int, 
		            int);
	void Shutdown();

private:

	int                                              m_textureWidth, 
		                                             m_textureHeight;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_renderTargetTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_depthStencilView;

	D3D11_VIEWPORT                                   m_viewport;
};