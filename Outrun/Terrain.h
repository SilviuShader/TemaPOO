#pragma once

class Camera;

class Terrain
{
public:

	Terrain(Camera*, ID3D11Device*, float, float, int, int);
	Terrain(const Terrain&);
	~Terrain();

	       void                         Update(float);

	inline float                        GetRoadWidth()     const { return m_roadWidth;     }
	inline float                        GetSideWidth()     const { return m_sideWidth;     }
	inline int                          GetSegmentLength() const { return m_segmentLength; }
	inline Texture2D*                   GetZMap()          const { return m_zMap.get();    }

	inline DirectX::SimpleMath::Vector2 GetBottomSegment() const { return m_bottomSegment; }
	inline DirectX::SimpleMath::Vector2 GetTopSegment()    const { return m_topSegment; }

private:

	float RandomFloat();

private:

	float                        m_cameraHeight;

	float                        m_roadWidth;
	float                        m_sideWidth;
	int                          m_segmentLength;
	int                          m_linesCount;

	DirectX::SimpleMath::Vector2 m_bottomSegment;
	DirectX::SimpleMath::Vector2 m_topSegment;

	std::unique_ptr<Texture2D> m_zMap;
};