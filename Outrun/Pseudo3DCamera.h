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
		TerrainShaderParameters(float, float, float, float, int, DirectX::SimpleMath::Vector2, DirectX::SimpleMath::Vector2, int);

	private:

		float              m_roadWidth;
		float              m_sideWidth;
		float              m_translation;
		float              m_drawDistance;

		int                m_segmentLength;
		float              m_segment1X;
		float              m_segment1Y;
		int                m_padding2;
		float              m_segment2X;
		float              m_segment2Y;

		int                m_screenHeight;
		int                m_padding3;
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

	void                                End(ID3D11RenderTargetView* const*,
		                                    ID3D11DepthStencilView*,
		                                    int) override;

	void                                DrawTerrain(Terrain*);

	inline void                         SetPosition(DirectX::SimpleMath::Vector3 position) { m_cameraPosition = position; }
	inline DirectX::SimpleMath::Vector3 GetPosition() const                                { return m_cameraPosition;     }

private:

	ID3D11Device*                                                           m_d3dDevice;


	int                                                                     m_linesDrawCount;
	float                                                                   m_cameraDepth;

	DirectX::SimpleMath::Vector3                                            m_cameraPosition;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>                               m_terrainPixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>                                    m_terrainShaderParams;
	TerrainShaderParameters                                                 m_terrainParameters;
};