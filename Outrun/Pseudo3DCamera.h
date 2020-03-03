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
		TerrainShaderParameters(int, int, int);

	private:

		int m_screenWidth;
		int m_screenHeight;
		int m_segmentLength;
		int m_padding;
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