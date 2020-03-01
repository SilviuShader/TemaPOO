#pragma once

class Texture2D
{
public:
	
	Texture2D(ID3D11Device*, 
		      std::string);

	Texture2D(ID3D11ShaderResourceView*,
		      CD3D11_TEXTURE2D_DESC);
	
	Texture2D(const Texture2D&);
	~Texture2D();

	inline ID3D11ShaderResourceView* GetShaderResourceView() { return m_shaderResourceView.Get(); }

	inline int                       GetWidth()              { return m_textureDesc.Width;        }
	inline int                       GetHeight()             { return m_textureDesc.Height;       }

private:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	CD3D11_TEXTURE2D_DESC                            m_textureDesc;
};