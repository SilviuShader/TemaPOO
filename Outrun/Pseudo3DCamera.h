#pragma once

class Pseudo3DCamera : public Camera
{
private:

	enum RunningBatch
	{
		None,
		Sprite,
		Polygon
	};

	class TerrainLineProjection
	{
	public:

		TerrainLineProjection(DirectX::SimpleMath::Vector2, float);

		inline DirectX::SimpleMath::Vector2 GetPosition() const { return m_position; }
		inline float                        GetWidth()    const { return m_width; }

	private:

		DirectX::SimpleMath::Vector2 m_position;
		float                        m_width;
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

	void DrawQuad(DirectX::SimpleMath::Vector4,
		          DirectX::SimpleMath::Vector2,
		          DirectX::SimpleMath::Vector2,
		          float,
		          float);

	void Begin2D() override;
	void End2D()   override;

	void BeginPolygon();
	void EndPolygon();

	void CreateWhiteTexture();

	TerrainLineProjection ProjectLine(Terrain*, 
		                              const Terrain::Line&, 
		                              int);

private:

	ID3D11Device*                                                           m_d3dDevice;


	int                                                                     m_linesDrawCount;
	float                                                                   m_cameraDepth;

	std::unique_ptr<Texture2D>                                              m_whiteTexture;
	std::unique_ptr<DirectX::BasicEffect>                                   m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor> > m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_inputLayout;

	RunningBatch                                                            m_runningBatch;

	DirectX::SimpleMath::Vector3                                            m_cameraPosition;
};