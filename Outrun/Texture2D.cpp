#include "pch.h"
#include "Texture2D.h"

using namespace std;

using namespace Microsoft::WRL;

using namespace DirectX;
using namespace DX;

Texture2D::Texture2D(ID3D11Device* device, 
	                 string        filename)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t> > converter;

    ComPtr<ID3D11Resource> resource;
    ThrowIfFailed(CreateWICTextureFromFile(device,
                                           converter.from_bytes(filename).c_str(),
                                           resource.GetAddressOf(),
                                           m_shaderResourceView.ReleaseAndGetAddressOf()));

    ComPtr<ID3D11Texture2D> texture;
    ThrowIfFailed(resource.As(&texture));
    texture->GetDesc(&m_textureDesc);
}

Texture2D::Texture2D(ID3D11ShaderResourceView* shaderResourceView, 
                     CD3D11_TEXTURE2D_DESC     textureDesc) :
    m_shaderResourceView(shaderResourceView),
    m_textureDesc(textureDesc)
{
}

Texture2D::Texture2D(const Texture2D& other)
{
}

Texture2D::~Texture2D()
{
    m_shaderResourceView.Reset();
}
