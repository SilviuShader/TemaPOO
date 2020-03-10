#pragma once

class Pseudo3DCamera : public Camera
{
private:

	// The member variables are private here because I don't want to fail the exam
	// but nobody would do this if he were to implement this.
	class TerrainShaderParameters
	{
	public:

		TerrainShaderParameters();
		TerrainShaderParameters(float, 
			                    float, 
			                    float, 
			                    float, 
			                    int, 
			                    DirectX::SimpleMath::Vector2, 
			                    float, 
			                    DirectX::SimpleMath::Vector2,
			                    int, 
			                    float);

	private:

		float              m_roadWidth;
		float              m_sideWidth;
		float              m_translation;
		float              m_drawDistance;

		int                m_segmentLength;
		float              m_segment1X;
		float              m_segment1Y;
		float              m_maxRoadX;
		float              m_segment2X;
		float              m_segment2Y;

		int                m_screenHeight;
		float              m_positionX;
	};

public:

	Pseudo3DCamera(ID3D11Device*,
		           ID3D11DeviceContext*,
	 	           int,
		           int,
		           int,
		           int,
		           int,
		           float);

	Pseudo3DCamera(const Pseudo3DCamera&);
	~Pseudo3DCamera();

	float       GetZ(int);

	void        DrawTerrain(Terrain*);

	inline void SetPositionX(float posX)            { m_positionX           = posX;        }
	inline void TranslateStripes(float translation) { m_stripesTranslation += translation; }

private:

	ID3D11Device*                                                           m_d3dDevice;


	int                                                                     m_linesDrawCount;
	float                                                                   m_cameraDepth;
	float                                                                   m_positionX;
	float                                                                   m_stripesTranslation;

	Microsoft::WRL::ComPtr<ID3D11PixelShader>                               m_terrainPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                    m_terrainShaderParams;
	TerrainShaderParameters                                                 m_terrainParameters;
};