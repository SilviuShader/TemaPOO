#include "pch.h"
#include "Terrain.h"

using namespace std;

using namespace DirectX::SimpleMath;

Terrain::Terrain(Camera*       camera,
	             ID3D11Device* d3dDevice,
	             int           roadWidth,
	             int           segmentLength,
	             int           linesCount) :
	
	m_roadWidth(roadWidth),
	m_segmentLength(segmentLength),
	m_linesCount(linesCount)

{
	int cameraWidth  = camera->GetWidth();
	int cameraHeight = camera->GetHeight();

	float* textureData = new float[cameraWidth * cameraHeight * 4];

	for (int i = 0; i < cameraHeight; i++)
	{
		for (int j = 0; j < cameraWidth; j++)
		{
			float& r = textureData[(i * cameraWidth * 4) + (j * 4)];
			float& g = textureData[(i * cameraWidth * 4) + (j * 4) + 1];
			float& b = textureData[(i * cameraWidth * 4) + (j * 4) + 2];
			float& a = textureData[(i * cameraWidth * 4) + (j * 4) + 3];

			float screenX = j - ((float)cameraWidth / 2.0f);
			float screenY = i - ((float)cameraHeight / 2.0f);
			screenY = -screenY;
			screenY /= (float)cameraHeight;

			float z = -1.0f / screenY;

			int div = (int)z / m_segmentLength;

			r = div % 2 ? 0.0f : 1.0f;
			g = z > 0 ? 1.0f : 0.0f;
			b = 0.0f;
			a = 1.0f;
		}
	}

	m_zMap = make_unique<Texture2D>(d3dDevice, cameraWidth, cameraHeight, textureData);
}

Terrain::Terrain(const Terrain& other)
{
}

Terrain::~Terrain()
{
}
