#include "pch.h"
#include "Pseudo3DCamera.h"

using namespace std;

using Microsoft::WRL::ComPtr;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using namespace DX;

using VertexType = VertexPositionColor;

Pseudo3DCamera::TerrainShaderParameters::TerrainShaderParameters()
{
}

Pseudo3DCamera::TerrainShaderParameters::TerrainShaderParameters(int sw, int sh, int stw) :
	m_screenWidth(sw),
	m_screenHeight(sh),
	m_segmentLength(stw)
{
}

Pseudo3DCamera::Pseudo3DCamera(ID3D11Device*        device,
							   ID3D11DeviceContext* deviceContext, 
							   int                  width, 
							   int                  height, 
							   int                  screenWidth, 
							   int                  screenHeight,
							   int                  linesDrawCount,
							   float                cameraDepth) :
	Camera(device, 
		   deviceContext, 
		   width, 
		   height, 
		   screenWidth, 
		   screenHeight),
	
	m_d3dDevice(device),
	m_linesDrawCount(linesDrawCount),
	m_cameraDepth(cameraDepth),
	m_cameraPosition(Vector3::Zero)
{
	m_cameraPosition.y = 1500;

	m_terrainParameters = TerrainShaderParameters(width, height, 1);

	static_assert(!(sizeof(TerrainShaderParameters) % 16), "TerrainShaderParameters needs to be 16 bytes aligned");
	
	auto blob = ReadData(L"TerrainPixelShader.cso");
	ThrowIfFailed(m_d3dDevice->CreatePixelShader(blob.data(), 
		                                         blob.size(), 
		                                         nullptr, 
		                                         m_terrainPixelShader.ReleaseAndGetAddressOf()));

	CD3D11_BUFFER_DESC cbDesc(sizeof(TerrainShaderParameters), D3D11_BIND_CONSTANT_BUFFER);
	D3D11_SUBRESOURCE_DATA initData;

	initData.pSysMem = &m_terrainParameters; 
	initData.SysMemPitch = sizeof(TerrainShaderParameters);

	ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbDesc, 
		                                    &initData, 
		                                    m_terrainShaderParams.ReleaseAndGetAddressOf()));
}

Pseudo3DCamera::Pseudo3DCamera(const Pseudo3DCamera& other) :
	Camera(other)
{
}

Pseudo3DCamera::~Pseudo3DCamera()
{
	m_terrainPixelShader.Reset();
	m_terrainShaderParams.Reset();
}

void Pseudo3DCamera::End(ID3D11RenderTargetView* const* renderTargetViews,
						 ID3D11DepthStencilView*        depthStencilView,
						 int                            numViews)
{
	Camera::End(renderTargetViews, depthStencilView, numViews);
}

void Pseudo3DCamera::DrawTerrain(Terrain* terrain)
{
	m_terrainParameters = TerrainShaderParameters(m_width, m_height, terrain->GetSegmentLength());

	m_d3dContext->UpdateSubresource(m_terrainShaderParams.Get(), 0, nullptr, &m_terrainParameters, sizeof(TerrainShaderParameters), 0);

	Begin2D([=]()
		{
			m_d3dContext->PSSetConstantBuffers(0, 1, m_terrainShaderParams.GetAddressOf());
			m_d3dContext->PSSetShader(m_terrainPixelShader.Get(), nullptr, 0);
		});

	DrawSprite(terrain->GetZMap(), Vector2(0.0f, 0.0f), nullptr, 0.0f, Vector2::One);
}