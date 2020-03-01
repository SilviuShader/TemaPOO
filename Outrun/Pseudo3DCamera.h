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

public:

	Pseudo3DCamera(ID3D11Device*,
		           ID3D11DeviceContext*,
	 	           int,
		           int,
		           int,
		           int,
		           float);

	Pseudo3DCamera(const Pseudo3DCamera&);
	~Pseudo3DCamera();

	void DrawQuad(DirectX::SpriteBatch*,
		          DirectX::SimpleMath::Vector4,
		          DirectX::SimpleMath::Vector2,
		          DirectX::SimpleMath::Vector2,
		          float,
		          float);

	void End(ID3D11RenderTargetView* const*,
		     ID3D11DepthStencilView*,
		     int) override;

private:

	void Begin2D() override;
	void End2D()   override;

	void BeginPolygon();
	void EndPolygon();

	void CreateWhiteTexture();

private:

	ID3D11Device*                                                           m_d3dDevice;

	int                                                                     m_fieldOfView;
	std::unique_ptr<Texture2D>                                              m_whiteTexture;
	std::unique_ptr<DirectX::BasicEffect>                                   m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor> > m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_inputLayout;

	RunningBatch                                                            m_runningBatch;
};