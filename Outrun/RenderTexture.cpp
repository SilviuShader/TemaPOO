#include "pch.h"
#include "RenderTexture.h"

using namespace Microsoft::WRL;
using namespace DirectX;

RenderTexture::RenderTexture(ComPtr<ID3D11Device> device, 
	                         int                  textureWidth, 
	                         int                  textureHeight) :
	m_renderTargetTexture(nullptr),
	m_renderTargetView(nullptr),
	m_shaderResourceView(nullptr),
	m_depthStencilBuffer(nullptr),
	m_depthStencilView(nullptr)
{
	Initialize(device, 
		       textureWidth, 
		       textureHeight);
}

RenderTexture::~RenderTexture()
{
	Shutdown();
}

void RenderTexture::SetRenderTarget(ComPtr<ID3D11DeviceContext> deviceContext)
{
	deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	deviceContext->RSSetViewports(1, &m_viewport);
}

void RenderTexture::ClearRenderTarget(ComPtr<ID3D11DeviceContext> deviceContext, 
	                                  float                       red, 
	                                  float                       green, 
	                                  float                       blue, 
	                                  float                       alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), 
		                                 color);

	deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), 
		                                 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		                                 1.0f, 
		                                 0);

	return;
}

void RenderTexture::Initialize(ComPtr<ID3D11Device> device, 
	                           int                  textureWidth, 
	                           int                  textureHeight)
{
	D3D11_TEXTURE2D_DESC            textureDesc;
	HRESULT                         result;
	D3D11_RENDER_TARGET_VIEW_DESC   renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC            depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC   depthStencilViewDesc;

	m_textureWidth  = textureWidth;
	m_textureHeight = textureHeight;

	ZeroMemory(&textureDesc, 
		       sizeof(textureDesc));

	textureDesc.Width            = textureWidth;
	textureDesc.Height           = textureHeight;
	textureDesc.MipLevels        = 1;
	textureDesc.ArraySize        = 1;
	textureDesc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage            = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags   = 0;
	textureDesc.MiscFlags        = 0;

	m_textureDesc = textureDesc;

	result = device->CreateTexture2D(&textureDesc, 
		                             NULL, 
		                             m_renderTargetTexture.ReleaseAndGetAddressOf());
	
	if (FAILED(result))
		throw GraphicsException({ device.Get(), m_renderTargetTexture.Get() });

	renderTargetViewDesc.Format             = textureDesc.Format;
	renderTargetViewDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateRenderTargetView(m_renderTargetTexture.Get(), 
		                                    &renderTargetViewDesc, 
		                                    m_renderTargetView.ReleaseAndGetAddressOf());
	if (FAILED(result))
		throw GraphicsException({ device.Get(), m_renderTargetTexture.Get(), m_renderTargetView.Get() });

	shaderResourceViewDesc.Format                    = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;

	result = device->CreateShaderResourceView(m_renderTargetTexture.Get(), 
		                                      &shaderResourceViewDesc, 
		                                      m_shaderResourceView.ReleaseAndGetAddressOf());
	
	if (FAILED(result))
		throw GraphicsException({ device.Get(), m_renderTargetTexture.Get(), m_shaderResourceView.Get() });

	ZeroMemory(&depthBufferDesc, 
		       sizeof(depthBufferDesc));

	depthBufferDesc.Width              = textureWidth;
	depthBufferDesc.Height             = textureHeight;
	depthBufferDesc.MipLevels          = 1;
	depthBufferDesc.ArraySize          = 1;
	depthBufferDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count   = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage              = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags     = 0;
	depthBufferDesc.MiscFlags          = 0;

	result = device->CreateTexture2D(&depthBufferDesc, 
		                             NULL, 
		                             m_depthStencilBuffer.ReleaseAndGetAddressOf());
	
	if (FAILED(result))
		throw GraphicsException({ device.Get(), m_depthStencilBuffer.Get() });

	ZeroMemory(&depthStencilViewDesc, 
		       sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(m_depthStencilBuffer.Get(), 
		                                    &depthStencilViewDesc, 
		                                    m_depthStencilView.ReleaseAndGetAddressOf());
	
	if (FAILED(result))
		throw GraphicsException({ device.Get(), m_depthStencilBuffer.Get(), m_depthStencilView.Get() });

	m_viewport.Width    = (float)textureWidth;
	m_viewport.Height   = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
}

void RenderTexture::Shutdown()
{
	m_depthStencilView.Reset();
	m_depthStencilBuffer.Reset();
	m_shaderResourceView.Reset();
	m_renderTargetView.Reset();
	m_renderTargetTexture.Reset();
}
