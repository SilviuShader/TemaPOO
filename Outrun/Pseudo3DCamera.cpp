#include "pch.h"
#include "Pseudo3DCamera.h"

using namespace std;

using Microsoft::WRL::ComPtr;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using VertexType = VertexPositionColor;

Pseudo3DCamera::TerrainLineProjection::TerrainLineProjection(Vector2 position, 
	                                                         float width) :
	m_position(position),
	m_width(width)
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
	m_runningBatch(Pseudo3DCamera::RunningBatch::None),
	m_cameraPosition(Vector3::Zero)
{
	CreateWhiteTexture();

	m_effect = make_unique<BasicEffect>(m_d3dDevice);
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, 
		                              &byteCodeLength);

	DX::ThrowIfFailed(m_d3dDevice->CreateInputLayout(VertexType::InputElements,
		                                             VertexType::InputElementCount,
		                                             shaderByteCode,
		                                             byteCodeLength,
		                                             m_inputLayout.ReleaseAndGetAddressOf()));

	m_batch = make_unique<PrimitiveBatch<VertexType> >(m_d3dContext);

	m_cameraPosition.y = 1500;
}

Pseudo3DCamera::Pseudo3DCamera(const Pseudo3DCamera& other) :
	Camera(other)
{
}

Pseudo3DCamera::~Pseudo3DCamera()
{
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
	m_whiteTexture.reset();
}

void Pseudo3DCamera::End(ID3D11RenderTargetView* const* renderTargetViews,
	                     ID3D11DepthStencilView*        depthStencilView,
	                     int                            numViews)
{
	EndPolygon();

	Camera::End(renderTargetViews, depthStencilView, numViews);
}

void Pseudo3DCamera::DrawTerrain(Terrain* terrain)
{
	//DrawQuad(Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(0.0f, -10.0f), 100, 20);

	vector<Terrain::Line>& terrainLines = terrain->GetLines();

	for (int i = 1; i < m_linesDrawCount; i++)
	{
		Terrain::Line& currentLine  = terrainLines[i       % terrainLines.size()];
		Terrain::Line& previousLine = terrainLines[(i - 1) % terrainLines.size()];

		TerrainLineProjection currentLineProj  = ProjectLine(terrain, currentLine);
		TerrainLineProjection previousLineProj = ProjectLine(terrain, previousLine);

		Vector4 grass = (i / 3) % 2 ? Vector4(0.3f, 0.7f, 0.5f, 1.0f) : Vector4(0.5f, 0.7f, 0.3f, 1.0f);

		Vector2 currentProjectedPos = currentLineProj.GetPosition();
		Vector2 previousProjectedPos = previousLineProj.GetPosition();

		DrawQuad(grass, Vector2(0.0f, previousProjectedPos.y), Vector2(0.0f, currentProjectedPos.y), m_width, m_width);
	}
}

void Pseudo3DCamera::DrawQuad(Vector4 color,
	                          Vector2 bottomPoint,
	                          Vector2 topPoint,
	                          float   bottomWidth,
	                          float   topWidth)
{
	m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);

	m_d3dContext->RSSetState(m_states->CullNone());

	m_effect->Apply(m_d3dContext);
	m_d3dContext->IASetInputLayout(m_inputLayout.Get());

	Vector2 bottomLeft = Vector2(bottomPoint.x - (bottomWidth / 2.0f), bottomPoint.y);
	Vector2 bottomRight = Vector2(bottomPoint.x + (bottomWidth / 2.0f), bottomPoint.y);
	Vector2 topLeft = Vector2(topPoint.x - (topWidth / 2.0f), topPoint.y);
	Vector2 topRight = Vector2(topPoint.x + (topWidth / 2.0f), topPoint.y);

	Vector2 vertices[] = { bottomLeft, bottomRight, topLeft, topRight };

	for (int i = 0; i < sizeof(vertices) / sizeof(Vector2); i++)
	{
		vertices[i].x /= (m_width / 2.0f);
		vertices[i].y /= (m_height / 2.0f);
		vertices[i].y = -vertices[i].y;
	}

	VertexPositionColor v1(Vector3(vertices[2].x, vertices[2].y, 0.5f), color);
	VertexPositionColor v2(Vector3(vertices[1].x, vertices[1].y, 0.5f), color);
	VertexPositionColor v3(Vector3(vertices[0].x, vertices[0].y, 0.5f), color);

	BeginPolygon();

	m_batch->DrawTriangle(v1, v2, v3);

	v1 = VertexPositionColor(Vector3(vertices[3].x, vertices[3].y, 0.5f), color);
	v2 = VertexPositionColor(Vector3(vertices[2].x, vertices[2].y, 0.5f), color);
	v3 = VertexPositionColor(Vector3(vertices[1].x, vertices[1].y, 0.5f), color);

	m_batch->DrawTriangle(v1, v2, v3);
}

void Pseudo3DCamera::Begin2D()
{
	switch (m_runningBatch)
	{
	case RunningBatch::None:
		break;
	case RunningBatch::Sprite:
		break;
	case RunningBatch::Polygon:
		EndPolygon();
		break;
	}

	Camera::Begin2D();
	m_runningBatch = RunningBatch::Sprite;
}

void Pseudo3DCamera::End2D()
{
	if (m_runningBatch == RunningBatch::Sprite)
	{
		Camera::End2D();
		m_runningBatch = RunningBatch::None;
	}
}

void Pseudo3DCamera::BeginPolygon()
{
	switch (m_runningBatch)
	{
	case RunningBatch::None:
		break;
	case RunningBatch::Sprite:
		End2D();
		break;
	case RunningBatch::Polygon:
		break;
	}

	if (m_runningBatch != RunningBatch::Polygon)
	{
		m_batch->Begin();
		m_runningBatch = RunningBatch::Polygon;
	}
}

void Pseudo3DCamera::EndPolygon()
{
	if (m_runningBatch == RunningBatch::Polygon)
	{
		m_batch->End();
		m_runningBatch = RunningBatch::None;
	}
}

void Pseudo3DCamera::CreateWhiteTexture()
{
	CD3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;

	float textureData[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	data.pSysMem = textureData;
	data.SysMemPitch = 4 * sizeof(float);
	data.SysMemPitch = 4 * sizeof(float);

	D3D11_SHADER_RESOURCE_VIEW_DESC resourceDesc;

	resourceDesc.Format = desc.Format;
	resourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MostDetailedMip = 0;
	resourceDesc.Texture2D.MipLevels = 1;

	ComPtr<ID3D11Texture2D>          resource;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	m_d3dDevice->CreateTexture2D(&desc, &data, resource.GetAddressOf());
	m_d3dDevice->CreateShaderResourceView(resource.Get(), &resourceDesc, shaderResourceView.GetAddressOf());

	m_whiteTexture = make_unique<Texture2D>(shaderResourceView.Get(), desc);
}

Pseudo3DCamera::TerrainLineProjection Pseudo3DCamera::ProjectLine(Terrain*             terrain,
	                                                              const Terrain::Line& line)
{
	Vector3 linePosition = line.GetPosition();
	Vector2 projectedPosition = Vector2();

	float scale = m_cameraDepth / (linePosition.z - m_cameraPosition.z);
	projectedPosition.x = (scale * (linePosition.x - m_cameraPosition.x)) * (m_width / 2.0f);
	projectedPosition.y = -(scale * (linePosition.y - m_cameraPosition.y)) * (m_height / 2.0f);

	float width = scale * (float)terrain->GetRoadWidth() * (m_width / 2.0f);

	return TerrainLineProjection(projectedPosition, width);
}