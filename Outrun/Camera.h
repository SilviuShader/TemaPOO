#pragma once

class Game;

class Camera
{
public:

	class CameraBeginFunction
	{
	public:

		CameraBeginFunction(std::function<void _cdecl()> = nullptr, 
			                int                          = 0);
		
		inline std::function<void _cdecl()> GetFunction() const { return m_function; }
		inline int                          GetID()       const { return m_id;       }

	private:

		int                          m_id;
	    std::function<void _cdecl()> m_function;
	};

public:
	
	Camera(Microsoft::WRL::ComPtr<ID3D11Device>,
		   Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
		   std::shared_ptr<Game>,
		   int, 
		   int, 
		   int, 
		   int);

	~Camera();

	void        Present(std::shared_ptr<DirectX::SpriteBatch>);
	void        OnScreenResize(int, 
                               int);

	void        DrawSprite(std::shared_ptr<Texture2D>,
		                   DirectX::SimpleMath::Vector2,
		                   const RECT*,
		                   float,
		                   DirectX::SimpleMath::Vector2);

	       void Begin(DirectX::SimpleMath::Vector4);

	       void End(int,
                    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>,
                    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>);

	       void Begin2D(std::shared_ptr<CameraBeginFunction> = nullptr);
	       void End2D();

	inline int  GetWidth()  { return m_width;  }
	inline int  GetHeight() { return m_height; }

protected:

	float GetPresentScale();

protected:

	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
	std::shared_ptr<Game>                       m_game;

	int                                         m_width;
	int                                         m_height;

	int                                         m_screenWidth;
	int                                         m_screenHeight;

	std::shared_ptr<CameraBeginFunction>        m_customBeginFunction;
	std::unique_ptr<DirectX::SpriteBatch>       m_spriteBatch;

private:

	bool                                   m_rendering2D;
	std::unique_ptr<DirectX::CommonStates> m_states;
	
	std::unique_ptr<RenderTexture>         m_renderTexture;
};
