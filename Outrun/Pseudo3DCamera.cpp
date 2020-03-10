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

Pseudo3DCamera::TerrainShaderParameters::TerrainShaderParameters(float   rw, 
	                                                             float   sw, 
	                                                             float   tr, 
	                                                             float   dd, 
	                                                             int     sl, 
	                                                             Vector2 segment1, 
	                                                             float   maxRoadX, 
	                                                             Vector2 segment2, 
	                                                             int     screenHeight, 
	                                                             float   positionX) :
	m_roadWidth(rw),
	m_sideWidth(sw),
	m_translation(tr),
	m_drawDistance(dd),
	m_segmentLength(sl),
	m_segment1X(segment1.x),
	m_segment1Y(segment1.y),
	m_maxRoadX(maxRoadX),
	m_segment2X(segment2.x),
	m_segment2Y(segment2.y),
	m_screenHeight(screenHeight),
	m_positionX(positionX)
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
	m_positionX(0.0f),
	m_stripesTranslation(0.0f),
	m_segmentLength(1.0f)
{

	m_terrainParameters = TerrainShaderParameters(0.0f, 0.0f, 0.0f, 0.0f, 0, Vector2(m_height, 0.0f), 0.0f, Vector2(0.0f, 0.0f), m_height, m_positionX);

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

float Pseudo3DCamera::GetZ(int line)
{
	float roadY = -1.0f;
	Vector2 screenPos = Vector2(0.0f, line / (float)(m_height - 1)) * 2.0f;
	screenPos -= Vector2(1.0f, 1.0f);
	screenPos.y = -screenPos.y;

	float z = roadY / screenPos.y;

	return z;
}

int Pseudo3DCamera::GetLine(float z)
{
	float roadY = -1.0f;
	Vector2 screenPos = Vector2(0.0f, 0.0f);
	screenPos.y = roadY / z;
	screenPos.y = -screenPos.y;
	screenPos += Vector2(1.0f, 1.0f);
	screenPos /= 2.0f;
	int line = screenPos.y * (float)(m_height - 1);

	return line;
}

void Pseudo3DCamera::DrawTerrain(Terrain* terrain)
{
	m_segmentLength = terrain->GetSegmentLength();

	m_terrainParameters = TerrainShaderParameters(terrain->GetRoadWidth(), 
		                                          terrain->GetSideWidth(), 
		                                          m_stripesTranslation,
		                                          m_cameraDepth, 
		                                          terrain->GetSegmentLength(), 
		                                          terrain->GetBottomSegment(), 
		                                          terrain->GetMaxRoadX(), 
		                                          terrain->GetTopSegment(), 
		                                          m_height, 
		                                          terrain->GetAccumulatedTranslation(m_height - 1) - m_positionX);

	m_d3dContext->UpdateSubresource(m_terrainShaderParams.Get(), 0, nullptr, &m_terrainParameters, sizeof(TerrainShaderParameters), 0);

	Begin2D([=]()
		{
			m_d3dContext->PSSetConstantBuffers(0, 1, m_terrainShaderParams.GetAddressOf());
			m_d3dContext->PSSetShader(m_terrainPixelShader.Get(), nullptr, 0);
		});

	DrawSprite(terrain->GetZMap(), Vector2(0.0f, 0.0f), nullptr, 0.0f, Vector2(m_width, 1.0f));
}

void Pseudo3DCamera::TranslateStripes(float translation)
{
	m_stripesTranslation += translation;

	// so that the values don't get to big.
	while (m_stripesTranslation > (2.0f * m_segmentLength))
		m_stripesTranslation -= (2.0f * m_segmentLength);
}
