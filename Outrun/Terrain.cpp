#include "pch.h"
#include "Terrain.h"

using namespace std;

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

Terrain::Terrain(GameObject*                  gameObject, 
	             shared_ptr<Texture2DManager> contentManager,
	             ComPtr<ID3D11Device>         d3dDevice) :

	GameComponent(gameObject),
	m_d3dDevice(d3dDevice),
	m_playerSpeed(0.0f),
	m_accumulatedTranslation(0.0f),
	m_maxRoadX(MIN_MAX_ROAD_X)

{
	float cameraHeight = m_parent->GetGame()->GetPseudo3DCamera()->GetHeight();
	m_objectsGenerator = make_unique<ObjectsGenerator>(contentManager);

	CreateTexture();

	m_topSegment       = Vector2(cameraHeight, 0.0f);
	m_bottomSegment    = Vector2(0.0f, 0.0f);
}

Terrain::~Terrain()
{
	m_objectsGenerator.reset();
	m_dataMap.reset();
	m_d3dDevice.Reset();
}

void Terrain::Update(float deltaTime)
{
	m_maxRoadX                              = MIN_MAX_ROAD_X;
	CreateTexture();

	Game*                      game         = m_parent->GetGame();
	shared_ptr<Pseudo3DCamera> camera       = game->GetPseudo3DCamera();
	float                      cameraHeight = camera->GetHeight();

	float prevBottomHeight                  = m_bottomSegment.x;

	m_topSegment.x    += deltaTime * ROAD_MOVE_SPEED * m_playerSpeed * camera->GetHeight();
	m_bottomSegment.x += deltaTime * ROAD_MOVE_SPEED * m_playerSpeed * camera->GetHeight();
	
	float bottomDifference = m_bottomSegment.x - prevBottomHeight;

	float alpha = (camera->GetWidth() / 2.0f) * (ROAD_MOVE_SPEED * 100.0f);
	m_accumulatedTranslation += GetRoadX(cameraHeight - 1) * alpha * (1.0f / abs(m_parent->GetGame()->GetRoadWidth())) * bottomDifference;

	float startY = cameraHeight * 1.0f;
	
	if (float diff = m_topSegment.x - startY; diff >= 0)
	{
		m_bottomSegment = m_topSegment;
		m_topSegment = Vector2(cameraHeight - startY, 
			                   Utils::RandomFloat() * (1.0f / (camera->GetHeight() * camera->GetHeight())) * MAX_CURVE_SLOPE * (rand() % 2 ? -1.0f : 1.0f));
	}

	m_objectsGenerator->Update(GetParent()->GetGame(),
		                       deltaTime);
}

void Terrain::Render()
{
	m_parent->GetGame()->GetPseudo3DCamera()->DrawTerrain(this);
}

float Terrain::GetRoadX(int crtHeight)
{
	shared_ptr<Pseudo3DCamera> camera = m_parent->GetGame()->GetPseudo3DCamera();
	float result = 0.0f;
	int topSegH = m_topSegment.x;
	int bottomSegH = m_bottomSegment.x;
	float z = camera->GetZ(crtHeight);

	int linesDiff = camera->GetHeight() - max(crtHeight, topSegH);
	float dx = m_bottomSegment.y;
	float ddx = dx * linesDiff * z * z;

	result = linesDiff * (dx + ddx) / 2.0f;

	if (crtHeight <= topSegH)
	{
		linesDiff = topSegH - crtHeight;
		dx = m_topSegment.y;
		float prevStr = ddx;
		ddx += dx * linesDiff * z * z;
		result += linesDiff * (prevStr + ddx) / 2.0f;
	}

	return result;
}

void Terrain::CreateTexture()
{
	Game*                      game         = m_parent->GetGame();
	shared_ptr<Pseudo3DCamera> camera       = game->GetPseudo3DCamera();
	int                        cameraHeight = camera->GetHeight();

	float* textureData = new float[cameraHeight * 4];

	for (int i = 0; i < cameraHeight; i++)
	{
		float& r = textureData[(i * 4)];
		float& g = textureData[(i * 4) + 1];
		float& b = textureData[(i * 4) + 2];
		float& a = textureData[(i * 4) + 3];

		float roadX = 0.0f;

		if (float z = camera->GetZ(i); z <= camera->GetCameraDepth() && z > 0)
			roadX = GetRoadX(i);

		r = roadX;
		// such a waste of memory, huh?
		// not gonna bother optimizing stuff like that
		g = 0.0f;
		b = 0.0f;
		a = 1.0f;

		m_maxRoadX = max(abs(roadX), m_maxRoadX);
	}

	// Now we normalize the values so that they are in range [0.0f, 1.0f] 
	for (int i = 0; i < cameraHeight; i++)
	{
		float& r = textureData[(i * 4)];
		r /= m_maxRoadX;
		// It worked perfectly fine without changing the range from [-1, 1] to [0, 1]
		// but it may not work on all devices so... better be safe and make the values positive
		r = (r + 1.0f) / 2.0f;
	}

	m_dataMap = make_shared<Texture2D>(m_d3dDevice, 1, cameraHeight, textureData);

	// Almost forgot this.. RIP
	delete[] textureData;
	textureData = NULL;
}
