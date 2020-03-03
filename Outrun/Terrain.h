#pragma once

class Camera;

class Terrain
{
public:

	Terrain(Camera*, ID3D11Device*,int, int, int);
	Terrain(const Terrain&);
	~Terrain();

	inline int        GetRoadWidth()     const { return m_roadWidth;     }
	inline int        GetSegmentLength() const { return m_segmentLength; }
	inline Texture2D* GetZMap()          const { return m_zMap.get();    }

private:

	int                        m_roadWidth;
	int                        m_segmentLength;
	int                        m_linesCount;

	std::unique_ptr<Texture2D> m_zMap;
};