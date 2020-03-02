//
// Game.h
//

#pragma once

#include "StepTimer.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game() noexcept;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

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

    std::unique_ptr<DirectX::Keyboard>               m_keyboard;
    std::unique_ptr<DirectX::Mouse>                  m_mouse;

    std::unique_ptr<DirectX::SpriteBatch>            m_spriteBatch;
    std::unique_ptr<DirectX::CommonStates>           m_states;

    std::unique_ptr<Pseudo3DCamera>                  m_mainCamera;
    std::unique_ptr<ContentManager>                  m_contentManager;
    std::unique_ptr<Terrain>                         m_terrain;

    std::shared_ptr<Texture2D>                       m_testTexture;
};
