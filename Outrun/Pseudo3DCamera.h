#pragma once

class Pseudo3DCamera : public Camera
{
private:

	const int TERRAIN_BEGIN_PIPELINE_ID = 1;

private:

	// The member variables are private here because I don't want to fail the exam
	// but nobody would do this if he were to implement this.

	class ConstantTerrainShaderParameters
	{
	public:

		ConstantTerrainShaderParameters(float, 
			                            float, 
			                            int, 
			                            float, 
			                            int, 
			                            float);

	private:

		float m_roadWidth;
		float m_sideWidth;
		int   m_screenHeight;
		float m_drawDistance;

		int   m_segmentLength;
		float m_roadHeight;
		int   m_padding1;
		int   m_padding2;
	};

	class TerrainShaderParameters
	{
	public:

		TerrainShaderParameters(float, 
			                    float,
			                    float);

	private:

		float m_maxRoadX;
		float m_translation;
		float m_positionX;
		int   m_padding1;
	};

public:

	Pseudo3DCamera(Microsoft::WRL::ComPtr<ID3D11Device>,
		           Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
		           std::shared_ptr<Game>,
	 	           int,
		           int,
		           int,
		           int,
		           float);

	~Pseudo3DCamera();

	       float GetZ(int);
	       int   GetLine(float);

	       void  DrawTerrain(Terrain*);


	       void  TranslateStripes(float translation);

	inline void  SetPositionX(float posX) { m_positionX = posX;   }
	inline float GetCameraDepth() const   { return m_cameraDepth; }

private:

	Microsoft::WRL::ComPtr<ID3D11Device>      m_d3dDevice;

	float                                     m_cameraDepth;
	float                                     m_positionX;
	float                                     m_stripesTranslation;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_terrainPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>      m_constantTerrainShaderParams;
	Microsoft::WRL::ComPtr<ID3D11Buffer>      m_terrainShaderParams;
	TerrainShaderParameters                   m_terrainParameters;
};