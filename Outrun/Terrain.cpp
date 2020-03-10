#include "pch.h"
#include "Terrain.h"

using namespace std;

using namespace DirectX::SimpleMath;

Terrain::Terrain(GameObject*   gameObject, 
	             Camera*       camera,
	             ID3D11Device* d3dDevice,
	             float         roadWidth,
	             float         sideWidth,
	             int           segmentLength,
	             int           linesCount) :

	GameComponent(gameObject),
	m_camera(camera),
	m_d3dDevice(d3dDevice),
	m_roadWidth(roadWidth),
	m_sideWidth(sideWidth),
	m_playerSpeed(0.0f),
	m_accumulatedTranslation(0.0f),
	m_segmentLength(segmentLength),
	m_linesCount(linesCount),
	m_maxRoadX(MIN_MAX_ROAD_X),
	m_accumultatedBottomDifference(0.0f)

{

	m_cameraHeight = m_camera->GetHeight();

	CreateTexture();

	m_topSegment = Vector2(m_cameraHeight, 0.0000f);
	m_bottomSegment = Vector2(0.0f, 0.0f);
}

Terrain::~Terrain()
{
}

void Terrain::Update(float deltaTime)
{
	m_maxRoadX = MIN_MAX_ROAD_X;
	CreateTexture();

	Game*           game   = m_parent->GetGame();
	Pseudo3DCamera* camera = game->GetPseudo3DCamera();

	float alpha = 10.0f;

	float prevBottomHeight = m_bottomSegment.x;

	m_topSegment.x    += deltaTime * alpha * m_playerSpeed;
	m_bottomSegment.x += deltaTime * alpha * m_playerSpeed;

	float startY = m_cameraHeight * 1.0f;

	float diff = m_topSegment.x - startY;
	
	m_accumultatedBottomDifference += m_bottomSegment.x - prevBottomHeight;

	float depthZ = camera->GetZ((m_cameraHeight / 2));
	m_accumulatedTranslation += GetRoadX(m_cameraHeight - 1, camera->GetZ(m_cameraHeight - 1)) * depthZ * m_accumultatedBottomDifference;
	m_accumultatedBottomDifference = 0.0f;

	if (diff >= 0)
	{
		m_bottomSegment = m_topSegment;
		m_topSegment = Vector2(m_cameraHeight - startY, RandomFloat() * 0.00015f * (rand() % 2 ? -1.0f : 1.0f));
	}
}

void Terrain::Render(Pseudo3DCamera* camera)
{
	camera->DrawTerrain(this);
}

float Terrain::GetRoadX(int crtHeight, float z)
{
	float result = 0.0f;
	int topSegH = m_topSegment.x;
	int bottomSegH = m_bottomSegment.x;

	int linesDiff = m_cameraHeight - max(crtHeight, topSegH);
	float dx = m_bottomSegment.y;
	float ddx = dx * linesDiff * z;

	result = linesDiff * (dx + ddx) / 2.0f;

	if (crtHeight <= topSegH)
	{
		linesDiff = topSegH - crtHeight;
		dx = m_topSegment.y;
		float prevStr = ddx;
		ddx += dx * linesDiff * z;
		result += linesDiff * (prevStr + ddx) / 2.0f;
	}

	return result;
}

float Terrain::RandomFloat()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

void Terrain::CreateTexture()
{
	Game*           game         = m_parent->GetGame();
	Pseudo3DCamera* camera       = game->GetPseudo3DCamera();
	int             cameraHeight = m_camera->GetHeight();

	float* textureData = new float[cameraHeight * 4];

	for (int i = 0; i < cameraHeight; i++)
	{
		float& r = textureData[(i * 4)];
		float& g = textureData[(i * 4) + 1];
		float& b = textureData[(i * 4) + 2];
		float& a = textureData[(i * 4) + 3];

		float z = camera->GetZ(i);
		float roadX = 0.0f;

		if (z <= 15.0f)
			roadX = GetRoadX(i, z);

		r = roadX;
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
	}

	m_zMap = make_unique<Texture2D>(m_d3dDevice, 1, cameraHeight, textureData);

	// Almost forgot this.. RIP
	delete[] textureData;
	textureData = NULL;
}
