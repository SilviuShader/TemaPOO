#pragma once

class Texture2D
{
public:
	
	Texture2D(Microsoft::WRL::ComPtr<ID3D11Device>, 
		      std::string);
	
	Texture2D(Microsoft::WRL::ComPtr<ID3D11Device>, 
		      int, 
		      int, 
		      float*);

	Texture2D(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>,
		      CD3D11_TEXTURE2D_DESC);

	~Texture2D();

	inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetShaderResourceView() const { return m_shaderResourceView; }

	inline int                                              GetWidth()              const { return m_textureDesc.Width;  }
	inline int                                              GetHeight()             const { return m_textureDesc.Height; }

private:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	CD3D11_TEXTURE2D_DESC                            m_textureDesc;
};