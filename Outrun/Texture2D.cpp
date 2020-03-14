#include "pch.h"
#include "Texture2D.h"

using namespace std;

using namespace Microsoft::WRL;

using namespace DirectX;
using namespace DX;

Texture2D::Texture2D(ComPtr<ID3D11Device> d3dDevice, 
	                 string               filename)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t> > converter;

    ComPtr<ID3D11Resource> resource;
    ThrowIfFailed(CreateWICTextureFromFile(d3dDevice.Get(),
                                           converter.from_bytes(filename).c_str(),
                                           resource.GetAddressOf(),
                                           m_shaderResourceView.ReleaseAndGetAddressOf()));

    ComPtr<ID3D11Texture2D> texture;
    ThrowIfFailed(resource.As(&texture));
    texture->GetDesc(&m_textureDesc);
}

Texture2D::Texture2D(ComPtr<ID3D11Device> d3dDevice, 
                     int                  width, 
                     int                  height, 
                     float*               textureData)
{
    float pixelsCount = width * height;

    CD3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA data;

    data.pSysMem = textureData;
    data.SysMemPitch = 4 * sizeof(float) * width;
    data.SysMemSlicePitch = 4 * sizeof(float);

    D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;

    resourceDesc.Format = desc.Format;
    resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    resourceDesc.Texture2D.MostDetailedMip = 0;
    resourceDesc.Texture2D.MipLevels = 1;

    ComPtr<ID3D11Texture2D>          resource;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    ThrowIfFailed(d3dDevice->CreateTexture2D(&desc, &data, resource.GetAddressOf()));
    
    ThrowIfFailed(d3dDevice->CreateShaderResourceView(resource.Get(), &resourceDesc, shaderResourceView.GetAddressOf()));

    m_shaderResourceView = shaderResourceView;
    m_textureDesc = desc;
}

Texture2D::~Texture2D()
{
    m_shaderResourceView.Reset();
}
