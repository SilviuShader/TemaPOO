//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using namespace DX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept :
    m_window(nullptr),
    m_outputWidth(900),
    m_outputHeight(900),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth  = max(width, 1);
    m_outputHeight = max(height, 1);

    m_keyboard = make_shared<Keyboard>();
    m_mouse    = make_shared<Mouse>();

    m_mouse->SetWindow(window);

    CreateDevice();

    CreateResources();
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    InputManager::GetInstance()->Update();
    if (InputManager::GetInstance()->GetKey(InputManager::GameKey::Esc))
    {
        switch (m_gameState)
        {
        case Game::GameState::Playing:
            m_gameState = Game::GameState::Pause;
            break;
        case Game::GameState::End:
            ExitGame();
            break;
        case Game::GameState::Pause:
            m_gameState = Game::GameState::Playing;
            break;
        }
    }

    switch (m_gameState)
    {
    case Game::GameState::Playing:

        for (shared_ptr<GameObject>& gameObj : m_gameObjects)
            gameObj->Update(elapsedTime);
        RemoveDeadObjects();

        m_collisionManager->Update(m_gameObjects);
        RemoveDeadObjects();

        // Wait wasn't the player removed from the list at this point?
        // Well, no, it's a smart pointer, the player is still there, event though it was removed from the list
        // This isn't a leak of any sort because it is completely removed in the ReleaseGameResources method.
        // also, the player is still needed for this step... same applies for terrain.
        if (m_player->GetParent()->Dead())
            m_gameState = Game::GameState::End;

        break;
    case Game::GameState::End:



        break;
    }
    // update UI
    for (int i = 0; i < (int)Game::GameState::Last; i++)
        if (i == (int)m_gameState)
            m_uiLayers[i]->SetActive(true);
        else
            m_uiLayers[i]->SetActive(false);

    for (int i = 0; i < (int)Game::GameState::Last; i++)
        m_uiLayers[i]->Update(m_uiCamera);
        
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    m_pseudo3DCamera->Begin(clearColor);

    for (shared_ptr<GameObject>& gameObj : m_gameObjects)
        gameObj->Render();

    m_pseudo3DCamera->End(1, 
                          m_renderTargetView,
                          m_depthStencilView);

    m_uiCamera->Begin(Vector4::Zero);

    for (int i = 0; i < (int)Game::GameState::Last; i++)
        m_uiLayers[i]->Render(m_uiCamera);
    
    m_uiCamera->End(1,
                    m_renderTargetView,
                    m_depthStencilView);

    Clear();

    m_spriteBatch->Begin(SpriteSortMode_Deferred, 
                         nullptr, 
                         m_states->PointClamp());


    m_pseudo3DCamera->Present(m_spriteBatch);
    m_uiCamera->Present(m_spriteBatch);

    m_spriteBatch->End();
    
    // Now that we finish rendering the texture,
    // we must let the computer render to it again
    ID3D11ShaderResourceView* const null[] = { nullptr, nullptr };
    m_d3dContext->PSSetShaderResources(0, 2, null);

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::Black);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, 
                               int height)
{
    m_outputWidth = max(width, 
                        1);

    m_outputHeight = max(height, 
                         1);

    CreateResources();

    if (m_pseudo3DCamera != nullptr)
        m_pseudo3DCamera->OnScreenResize(m_outputWidth, 
                                         m_outputHeight);

    if (m_uiCamera != nullptr)
        m_uiCamera->OnScreenResize(m_outputWidth, 
                                   m_outputHeight);
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    width  = 640;
    height = 480;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(D3D11CreateDevice(
        nullptr,                            // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),    // returns the Direct3D device created
        &m_featureLevel,                    // returns feature level of device created
        context.ReleaseAndGetAddressOf()    // returns the device immediate context
        ));

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(device.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    DX::ThrowIfFailed(device.As(&m_d3dDevice));
    DX::ThrowIfFailed(context.As(&m_d3dContext));

    InputManager::CreateInstance(m_keyboard, 
                                 m_mouse);

    m_spriteBatch = make_shared<SpriteBatch>(m_d3dContext.Get());
    m_states = make_unique<CommonStates>(m_d3dDevice.Get());

    CreateGameResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = backBufferCount;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a SwapChain from a Win32 window.
        DX::ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
            m_d3dDevice.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            m_swapChain.ReleaseAndGetAddressOf()
            ));

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    ReleaseGameResources();

    m_states.reset();
    m_spriteBatch.reset();

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain.Reset();
    m_d3dContext.Reset();
    m_d3dDevice.Reset();

    InputManager::Reset();

    CreateDevice();
    CreateResources();
}

void Game::RemoveDeadObjects()
{
    m_gameObjects.remove_if([](const shared_ptr<GameObject>& gameObj)
        {
            if (gameObj->Dead())
                gameObj->ClearComponents();

            return gameObj->Dead();
        });
}

void Game::CreateGameResources()
{
    m_gameState                               = Game::GameState::Playing;

    m_contentManager                          = make_shared<ContentManager>(m_d3dDevice,
                                                                            "Resources/");

    m_pseudo3DCamera                          = make_shared<Pseudo3DCamera>(m_d3dDevice,
                                                                            m_d3dContext,
                                                                            shared_from_this(),
                                                                            GAME_WIDTH,
                                                                            GAME_HEIGHT,
                                                                            m_outputWidth,
                                                                            m_outputHeight,
                                                                            CAMERA_DEPTH);

    m_gameObjects                             = list<shared_ptr<GameObject> >();

    m_collisionManager                        = make_unique<CollisionManager>();
    
    // Add the terrain
    shared_ptr<GameObject> terrainObj         = make_shared<GameObject>(shared_from_this());

    m_terrain                                 = make_shared<Terrain>(terrainObj,
                                                                     m_contentManager,
                                                                     m_d3dDevice);

    terrainObj->AddComponent(m_terrain);
    m_gameObjects.push_back(terrainObj);

    // Add the player
    shared_ptr<GameObject> playerObj          = make_shared<GameObject>(shared_from_this());
    m_player                                  = make_shared<Player>(playerObj);
    playerObj->AddComponent(m_player);

    m_carTexture                              = m_contentManager->Load<Texture2D>("Car.png");

    shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(playerObj,  
                                                                            m_carTexture);
    playerObj->AddComponent(spriteRenderer);

    m_gameObjects.push_back(playerObj);

    CreateUI();
}

void Game::CreateUI()
{
    const float MARGIN = 10.0f;

    m_uiCamera = make_shared<UICamera>(m_d3dDevice, 
                                       m_d3dContext, 
                                       shared_from_this(), 
                                       GAME_WIDTH, 
                                       GAME_HEIGHT, 
                                       m_outputWidth, 
                                       m_outputHeight);

    for (int i = 0; i < (int)Game::GameState::Last; i++)
        m_uiLayers[i] = make_shared<UILayer>(Vector2::Zero, 
                                             Vector2(GAME_WIDTH, 
                                                     GAME_HEIGHT));

    shared_ptr<Texture2D> replayTexture        = m_contentManager->Load<Texture2D>("UI/Replay.png");
    shared_ptr<Texture2D> replayPressedTexture = m_contentManager->Load<Texture2D>("UI/ReplayPressed.png");

    shared_ptr<Texture2D> quitTexture          = m_contentManager->Load<Texture2D>("UI/Quit.png");
    shared_ptr<Texture2D> quitPressedTexture   = m_contentManager->Load<Texture2D>("UI/QuitPressed.png");

    shared_ptr<Texture2D> scoreBarTexture      = m_contentManager->Load<Texture2D>("UI/ScoreBar.png");

    shared_ptr<GameFont> vcr32FontBlack        = m_contentManager->Load<GameFont>("Fonts/VCR32.spritefont");
    // make the vcr32FontBlack the color black.
    vcr32FontBlack->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    shared_ptr<UIButton> endReplayButton = make_shared<UIButton>(Vector2((-replayTexture->GetWidth() / 2.0f) - MARGIN, 
                                                                         replayTexture->GetHeight() + 3.0f * MARGIN), 
                                                                 Vector2(replayTexture->GetWidth(),
                                                                         replayTexture->GetHeight()),
                                                                 replayTexture,
                                                                 replayPressedTexture,
                                                                 bind(&Game::OnReplayButtonReleased, this));

    m_uiLayers[(int)Game::GameState::End]->AddChild(endReplayButton);

    shared_ptr<UIButton> endQuitButton = make_shared<UIButton>(Vector2((quitTexture->GetWidth() / 2.0f) + MARGIN,
                                                                       quitTexture->GetHeight() + 3.0f * MARGIN),
                                                               Vector2(quitTexture->GetWidth(),
                                                               quitTexture->GetHeight()),
                                                               quitTexture,
                                                               quitPressedTexture,
                                                               ExitGame);

    m_uiLayers[(int)Game::GameState::End]->AddChild(endQuitButton);

    shared_ptr<UIImage> endHighScoreImage = make_shared<UIImage>(scoreBarTexture,
                                                                 Vector2(0.0f,
                                                                         (-GAME_HEIGHT / 2.0f) + (scoreBarTexture->GetHeight() / 2.0f)),
                                                                 Vector2(scoreBarTexture->GetWidth(), 
                                                                         scoreBarTexture->GetHeight()));

    m_uiLayers[(int)Game::GameState::End]->AddChild(endHighScoreImage);
    shared_ptr<UIText> endBestScoreText = make_shared<UIText>(vcr32FontBlack,
                                                              Vector2(0.0f,
                                                                      -MARGIN / 4.0f));
    endBestScoreText->SetText("BEST SCORE");
    endHighScoreImage->AddChild(dynamic_pointer_cast<UIElement>(endBestScoreText));
}

void Game::ReleaseGameResources()
{
    for (int i = 0; i < (int)Game::GameState::Last; i++)
        m_uiLayers[i].reset();

    m_uiCamera.reset();
        
    m_carTexture.reset();
    m_player.reset();
    m_terrain.reset();

    m_collisionManager.reset();

    for (auto& gameObject : m_gameObjects)
    {
        if (gameObject != nullptr)
        {
            gameObject->ClearComponents();
            gameObject.reset();
        }
    }
    m_gameObjects.clear();

    m_carTexture.reset();
    m_pseudo3DCamera.reset();
    m_contentManager.reset();
}

void Game::OnReplayButtonReleased()
{
    ReleaseGameResources();
    CreateGameResources();
    m_gameState = Game::GameState::Playing;
}
