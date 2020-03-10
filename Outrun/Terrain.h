#pragma once

class Terrain : public GameComponent
{
private:

	const float MIN_MAX_ROAD_X = 1.0f;

public:

	Terrain(GameObject*, Camera*, ID3D11Device*, float, float, int, int);
	~Terrain();

	       void                         Update(float)           override;
		   void                         Render(Pseudo3DCamera*) override;

	inline void                         SetPlayerSpeed(float speed)       { m_playerSpeed = speed;           }

	inline float                        GetRoadWidth()              const { return m_roadWidth;              }
	inline float                        GetSideWidth()              const { return m_sideWidth;              }
	inline float                        GetMaxRoadX()               const { return m_maxRoadX;               }
	inline float                        GetAccumulatedTranslation() const { return m_accumulatedTranslation; }
	inline int                          GetSegmentLength()          const { return m_segmentLength;          }
	inline Texture2D*                   GetZMap()                   const { return m_zMap.get();             }

	inline DirectX::SimpleMath::Vector2 GetBottomSegment()          const { return m_bottomSegment;          }
	inline DirectX::SimpleMath::Vector2 GetTopSegment()             const { return m_topSegment;             }

private:

	float GetRoadX(int, float);
	float RandomFloat();
	void  CreateTexture();

private:

	Camera*                      m_camera;
	ID3D11Device*                m_d3dDevice;

	float                        m_cameraHeight;

	float                        m_roadWidth;
	float                        m_sideWidth;
	float                        m_playerSpeed;
	float                        m_maxRoadX;
	float                        m_accumulatedTranslation;
	int                          m_segmentLength;
	int                          m_linesCount;


	DirectX::SimpleMath::Vector2 m_bottomSegment;
	DirectX::SimpleMath::Vector2 m_topSegment;

	std::unique_ptr<Texture2D>   m_zMap;

	float                        m_accumultatedBottomDifference;
};