#include "pch.h"
#include "Pseudo3DCamera.h"

using namespace std;

using namespace Microsoft::WRL;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using namespace DX;

using VertexType = VertexPositionColor;

Pseudo3DCamera::ConstantTerrainShaderParameters::ConstantTerrainShaderParameters(float roadWidth, 
	                                                                             float sideWidth, 
	                                                                             int   screenHeight, 
	                                                                             float drawDistance, 
	                                                                             int   segmentLength, 
	                                                                             float roadHeight) :
	m_roadWidth(roadWidth),
	m_sideWidth(sideWidth),
	m_screenHeight(screenHeight),
	m_drawDistance(drawDistance),
	m_segmentLength(segmentLength),
	m_roadHeight(roadHeight),
	m_padding1(0),
	m_padding2(0)
{
}


Pseudo3DCamera::TerrainShaderParameters::TerrainShaderParameters(float maxRoadX, 
	                                                             float translation,
	                                                             float positionX) :
	m_maxRoadX(maxRoadX),
	m_translation(translation),
	m_positionX(positionX),
	m_padding1(0)

{
}

Pseudo3DCamera::TerrainShaderParameters& Pseudo3DCamera::TerrainShaderParameters::operator=(Vector3 vec)
{
	m_maxRoadX    = vec.x;
	m_translation = vec.y;
	m_positionX   = vec.z;

	return *this;
}

Pseudo3DCamera::Pseudo3DCamera(ComPtr<ID3D11Device>        device,
							   ComPtr<ID3D11DeviceContext> deviceContext, 
	                           Game*                       game,
							   int                         width, 
							   int                         height, 
							   int                         screenWidth, 
							   int                         screenHeight,
							   float                       cameraDepth) :
	Camera(device, 
		   deviceContext,
		   game,
		   width, 
		   height, 
		   screenWidth, 
		   screenHeight),
	
	m_d3dDevice(device),
	m_cameraDepth(cameraDepth),
	m_positionX(0.0f),
	m_stripesTranslation(0.0f),
	m_terrainParameters(TerrainShaderParameters(1.0f, 0.0f, 0.0f))
{
	// To create the "custom shaders" pipeline I followed this tutorial:
	// https://github.com/microsoft/DirectXTK/wiki/Writing-custom-shaders
	// That's pretty much the only way to do this, so it's not copying.

	static_assert(!(sizeof(ConstantTerrainShaderParameters) % 16), 
		          "ConstantTerrainShaderParameters needs to be 16 bytes aligned");
	
	static_assert(!(sizeof(TerrainShaderParameters) % 16), 
		          "TerrainShaderParameters needs to be 16 bytes aligned");

	FileManager::GetInstance()->PushToLog("Creating Pseudo3DCamera");
	
	auto blob = ReadData(L"TerrainPixelShader.cso");
	ThrowIfFailed(m_d3dDevice->CreatePixelShader(blob.data(), 
		                                         blob.size(), 
		                                         nullptr, 
		                                         m_terrainPixelShader.ReleaseAndGetAddressOf()));

	ConstantTerrainShaderParameters constantTerrainShaderParameters(m_game->GetRoadWidth(), 
		                                                            m_game->GetSideWidth(), 
		                                                            m_height, 
		                                                            m_cameraDepth, 
		                                                            m_game->GetSegmentLength(), 
		                                                            m_game->GetRoadHeight());

	CD3D11_BUFFER_DESC     cbDesc(sizeof(ConstantTerrainShaderParameters), D3D11_BIND_CONSTANT_BUFFER);
	D3D11_SUBRESOURCE_DATA initData;

	initData.pSysMem     = &constantTerrainShaderParameters;
	initData.SysMemPitch = sizeof(ConstantTerrainShaderParameters);

	ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbDesc, 
		                                    &initData, 
		                                    m_constantTerrainShaderParams.ReleaseAndGetAddressOf()));

	CD3D11_BUFFER_DESC     cbDesc2(sizeof(TerrainShaderParameters), D3D11_BIND_CONSTANT_BUFFER);
	D3D11_SUBRESOURCE_DATA initData2;

	initData2.pSysMem     = &m_terrainParameters;
	initData2.SysMemPitch = sizeof(TerrainShaderParameters);

	ThrowIfFailed(m_d3dDevice->CreateBuffer(&cbDesc2,
		                                    &initData2,
		                                    m_terrainShaderParams.ReleaseAndGetAddressOf()));
}

Pseudo3DCamera::~Pseudo3DCamera()
{
	m_terrainPixelShader.Reset();
	m_terrainShaderParams.Reset();
	m_constantTerrainShaderParams.Reset();
	m_d3dDevice.Reset();
}

float Pseudo3DCamera::GetZ(int line)
{
	float roadY = m_game->GetRoadHeight();
	Vector2 screenPos = Vector2(0.0f, line / (float)(m_height - 1)) * 2.0f;
	screenPos -= Vector2(1.0f, 1.0f);
	screenPos.y = -screenPos.y;

	float z = roadY / screenPos.y;

	return z;
}

int Pseudo3DCamera::GetLine(float z)
{
	float roadY = m_game->GetRoadHeight();
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
	m_terrainParameters = Vector3(terrain->GetMaxRoadX(),
		                          m_stripesTranslation,
		                          terrain->GetAccumulatedTranslation() - m_positionX);

	m_d3dContext->UpdateSubresource(m_terrainShaderParams.Get(), 0, nullptr, &m_terrainParameters, sizeof(TerrainShaderParameters), 0);

	shared_ptr<Camera::CameraBeginFunction> customFunction = make_shared<Camera::CameraBeginFunction>(([=]()
		{
			m_d3dContext->PSSetConstantBuffers(0, 1, m_constantTerrainShaderParams.GetAddressOf());
			m_d3dContext->PSSetConstantBuffers(1, 1, m_terrainShaderParams.GetAddressOf());
			m_d3dContext->PSSetShader(m_terrainPixelShader.Get(), nullptr, 0);
		}), 
		TERRAIN_BEGIN_PIPELINE_ID);

	Begin2D(customFunction);

	DrawSprite(terrain->GetDataMap(), Vector2(0.0f, 0.0f), nullptr, 0.0f, Vector2(m_width, 1.0f));
}

void Pseudo3DCamera::TranslateStripes(float translation)
{
	m_stripesTranslation += translation;

	// so that the values don't get to big.
	while (m_stripesTranslation > (2.0f * m_game->GetSegmentLength()))
		m_stripesTranslation -= (2.0f * m_game->GetSegmentLength());
}