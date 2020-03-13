#pragma once

class Terrain : public GameComponent
{
private:

	const float MIN_MAX_ROAD_X  = 1.0f;
	const float ROAD_MOVE_SPEED = 0.03f;
	const float MAX_CURVE_SLOPE = 5.0f;

public:

	Terrain(std::shared_ptr<GameObject>,
		    std::shared_ptr<ContentManager>,
		    Microsoft::WRL::ComPtr<ID3D11Device>);

	~Terrain();

	       void                         Update(float)               override;
		   void                         Render()                    override;

	inline void                         SetPlayerSpeed(float speed)       { m_playerSpeed = speed;                  }

	inline float                        GetMaxRoadX()               const { return m_maxRoadX;                      }
	inline float                        GetAccumulatedTranslation() const { return m_accumulatedTranslation; }
	inline std::shared_ptr<Texture2D>   GetDataMap()                const { return m_dataMap;                       }

	inline DirectX::SimpleMath::Vector2 GetBottomSegment()          const { return m_bottomSegment;                 }
	inline DirectX::SimpleMath::Vector2 GetTopSegment()             const { return m_topSegment;                    }

	       float                        GetRoadX(int);

private:

	float RandomFloat();
	void  CreateTexture();

private:

	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;

	float                                m_playerSpeed;
	float                                m_maxRoadX;
	float                                m_accumulatedTranslation;

	DirectX::SimpleMath::Vector2         m_bottomSegment;
	DirectX::SimpleMath::Vector2         m_topSegment;

	std::shared_ptr<Texture2D>           m_dataMap;
	std::unique_ptr<ObjectsGenerator>    m_objectsGenerator;
};