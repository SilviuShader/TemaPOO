#include "pch.h"
#include "Terrain.h"

using namespace std;

using namespace DirectX::SimpleMath;

Terrain::Terrain(Camera*       camera,
	             ID3D11Device* d3dDevice,
	             float         roadWidth,
	             float         sideWidth,
	             int           segmentLength,
	             int           linesCount) :

	m_roadWidth(roadWidth),
	m_sideWidth(sideWidth),
	m_segmentLength(segmentLength),
	m_linesCount(linesCount)

{
	int cameraWidth  = camera->GetWidth();
	int cameraHeight = camera->GetHeight();

	m_cameraHeight = cameraHeight;

	float* textureData = new float[cameraWidth * cameraHeight * 4];

	for (int i = 0; i < cameraHeight; i++)
	{
		for (int j = 0; j < cameraWidth; j++)
		{
			float& r = textureData[(i * cameraWidth * 4) + (j * 4)];
			float& g = textureData[(i * cameraWidth * 4) + (j * 4) + 1];
			float& b = textureData[(i * cameraWidth * 4) + (j * 4) + 2];
			float& a = textureData[(i * cameraWidth * 4) + (j * 4) + 3];

			r = 1.0f;
			g = 0.0f;
			b = 0.0f;
			a = 1.0f;
		}
	}

	m_zMap = make_unique<Texture2D>(d3dDevice, cameraWidth, cameraHeight, textureData);
	m_topSegment = Vector2(cameraHeight, 0.0002f);
	m_bottomSegment = Vector2(0.0f, -0.0002f);
}

Terrain::Terrain(const Terrain& other)
{
}

Terrain::~Terrain()
{
}

void Terrain::Update(float deltaTime)
{
	float alpha = 100.0f;
	m_topSegment.x += deltaTime * alpha;
	m_bottomSegment.x += deltaTime * alpha;

	float diff = m_topSegment.x - m_cameraHeight;

	if (diff >= 0)
	{
		m_bottomSegment = m_topSegment;
		m_topSegment = Vector2(diff + (m_cameraHeight / 2.0f), RandomFloat() * 0.0002f * (rand() % 2 ? -1.0f : 1.0f));
	}
}

float Terrain::RandomFloat()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
