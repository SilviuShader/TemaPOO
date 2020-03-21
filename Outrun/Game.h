//
// Game.h
// This class is part of this template https://github.com/walbourn/directx-vs-templates/raw/master/VSIX/Direct3DUWPGame.vsix
// Of course, I have changed lots, but that's how this class started.
//

#pragma once

#include "StepTimer.h"

class Game : public std::enable_shared_from_this<Game>
{
private:

    enum class GameState
    {
        Playing,
        End,
        Pause,
        Last
    };

private:

    const int   GAME_WIDTH             = 320;
    const int   GAME_HEIGHT            = 240;

    // 3D Space properties
    const float CAMERA_DEPTH           = 15.0f;
    const float ROAD_WIDTH             = 1.0f;
    const float SIDE_WIDTH             = 0.05f;
    const int   SEGMENT_LENGTH         = 1;

    const float OBJECT_DISAPPEAR_DEPTH = 0.5f;
    // no matter how cool it looks when this value is -0.5
    // the car is too big in that case.
    const float ROAD_HEIGHT            = -1.0f;

public:

    Game() noexcept;

    // Initialization and management
    void                                            Initialize(HWND, 
                                                               int,
                                                               int);

    // Basic game loop
    void                                            Tick();

    // Messages
    void                                            OnActivated();
    void                                            OnDeactivated();
    void                                            OnSuspending();
    void                                            OnResuming();
    void                                            OnWindowSizeChanged(int, 
                                                                        int);

    // Properties
    void                                            GetDefaultSize(int&, 
                                                                   int&) const;

    // Getters
    inline std::shared_ptr<Pseudo3DCamera>          GetPseudo3DCamera()          const { return m_pseudo3DCamera;       }
    inline std::shared_ptr<Terrain>                 GetTerrain()                 const { return m_terrain;              }
    inline std::shared_ptr<Player>                  GetPlayer()                  const { return m_player;               }
    inline float                                    GetObjectDisappearDepth()    const { return OBJECT_DISAPPEAR_DEPTH; }
    inline float                                    GetRoadWidth()               const { return ROAD_WIDTH;             }
    inline float                                    GetRoadHeight()              const { return ROAD_HEIGHT;            }
    inline float                                    GetSideWidth()               const { return SIDE_WIDTH;             }
    inline float                                    GetSegmentLength()           const { return SEGMENT_LENGTH;         }

    inline std::list<std::shared_ptr<GameObject> >& GetGameObjects()                   { return m_gameObjects;    }

private:

    void Update(DX::StepTimer const&);
    void Render();

    void Clear();
    void SetRenderTarget();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    void ChangeGameState(Game::GameState);

    void RemoveDeadObjects();
    void CreateGameResources();
    void CreateUI();
    void CreateScoreUI(std::shared_ptr<UILayer>, 
                       float);
    void UpdateBestScoreLabels();
    void ReleaseGameResources();

    // callbacks
    void OnReplayButtonReleased();

private:

    // Device resources.
    HWND                                             m_window;
    int                                              m_outputWidth;
    int                                              m_outputHeight;

    D3D_FEATURE_LEVEL                                m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device1>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>     m_d3dContext;

    Microsoft::WRL::ComPtr<IDXGISwapChain1>          m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_depthStencilView;

    // Rendering loop timer.
    DX::StepTimer                                    m_timer;

    std::shared_ptr<DirectX::Keyboard>               m_keyboard;
    std::shared_ptr<DirectX::Mouse>                  m_mouse;

    std::shared_ptr<DirectX::SpriteBatch>            m_spriteBatch;
    std::unique_ptr<DirectX::CommonStates>           m_states;

    Game::GameState                                  m_gameState;

    std::shared_ptr<ContentManager>                  m_contentManager;
    std::shared_ptr<Pseudo3DCamera>                  m_pseudo3DCamera;
    std::shared_ptr<BloomCamera>                     m_bloomCamera;
    std::list<std::shared_ptr<GameObject> >          m_gameObjects;
    std::unique_ptr<CollisionManager>                m_collisionManager;

    std::shared_ptr<Terrain>                         m_terrain;
    std::shared_ptr<Player>                          m_player;

    std::shared_ptr<Texture2D>                       m_carTexture;
    std::shared_ptr<Texture2D>                       m_sunTexture;

    // UI
    std::shared_ptr<UICamera>                        m_uiCamera;
    std::shared_ptr<UILayer>                         m_uiLayers[(int)Game::GameState::Last];
    std::shared_ptr<UIImage>                         m_speedPointerImage;
    std::shared_ptr<UIText>                          m_distanceText;
    std::vector<std::shared_ptr<UIText> >            m_scoreLabels;
    std::vector<std::shared_ptr<UIText> >            m_bestScoreLabels;
    std::shared_ptr<UIText>                          m_livesLabel;
};
