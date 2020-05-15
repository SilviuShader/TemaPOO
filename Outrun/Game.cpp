//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();
extern void QuitGame();

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
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif // _DEBUG
}

Game::~Game()
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

    try
    {

        InputManager::GetInstance()->Update();
        if (InputManager::GetInstance()->GetKey(InputManager::GameKey::Esc))
        {
            switch (m_gameState)
            {
            case Game::GameState::Playing:
                ChangeGameState(Game::GameState::Pause);
                break;
            case Game::GameState::End:
                ExitGame();
                break;
            case Game::GameState::Pause:
                ChangeGameState(Game::GameState::Playing);
                break;
            }
        }

        if (m_gameState == GameState::Exiting)
        {
            QuitGame();
            return;
        }

        switch (m_gameState)
        {
        case Game::GameState::Playing:

            for (shared_ptr<GameObject>& gameObj : m_gameObjects)
                gameObj->Update(elapsedTime);

            m_collisionManager->Update(m_gameObjects);
            
            (*m_speedPointerImage.get()) = Utils::Lerp(GetPlayer()->GetSpeed() / GetPlayer()->GetMaxSpeed(),
                                                       -1.75f,
                                                       1.1f);

            m_distanceText->SetText(to_string((int)GetPlayer()->GetDistance()));
            for (int i = 0; i < m_scoreLabels.size(); i++)
                m_scoreLabels[i]->SetText(to_string((int)GetPlayer()->GetDistance()));

            m_livesLabel->SetText(to_string(GetPlayer()->GetLivesCount()));

            // Wait wasn't the player removed from the list at this point?
            // Well, no, it's a smart pointer, the player is still there, event though it was removed from the list
            // This isn't a leak of any sort because it is completely removed in the ReleaseGameResources method.
            // also, the player is still needed for this step... same applies for terrain.
            if (m_player->Dead())
            {
                int bestScore = FileManager::GetInstance()->ReadScore();
                bestScore = max(bestScore, (int)GetPlayer()->GetDistance());
                FileManager::GetInstance()->WriteScore(bestScore);

                ChangeGameState(Game::GameState::End);

                UpdateBestScoreLabels();
                m_player.reset();
                m_player = nullptr;
            }

            RemoveDeadObjects();

            break;
        case Game::GameState::End:

            break;

        case Game::GameState::Reseting:

            ReleaseGameResources();
            CreateGameResources();
            ChangeGameState(Game::GameState::Playing);

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
    catch (exception e)
    {
        FileManager::GetInstance()->PushToLog("Error while updating frame: " + to_string(timer.GetFrameCount()));
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }
        
}

// Draws the scene.
void Game::Render()
{
    if (m_gameState == GameState::Exiting)
        return;
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
        return;

    try
    {
        ID3D11ShaderResourceView* const null[] = { nullptr, nullptr };
        Vector4 clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

        m_pseudo3DCamera->Begin(clearColor);

        /*
        m_pseudo3DCamera->DrawSprite(m_sunTexture, 
                                     Vector2::Zero, 
                                     nullptr, 
                                     0.0f, 
                                     Vector2::One);
        m_pseudo3DCamera->End2D();
        */
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

        BloomCamera::BloomPresets currentBloomPreset = m_bloomCamera->GetBloomPreset();

        m_bloomCamera->SetBloomPreset(BloomCamera::BloomPresets::Blurry);
        m_bloomCamera->Bloom(m_pseudo3DCamera->GetTexture(),
            m_renderTargetView,
            m_depthStencilView);

        m_bloomCamera->SetBloomPreset(currentBloomPreset);

        m_d3dContext->PSSetShaderResources(0, 2, null);

        Clear();

        m_spriteBatch->Begin(SpriteSortMode_Deferred,
            nullptr,
            m_states->LinearClamp());

        float scale = 1.0f;
        
        if (auto [gameAspectRatio, windowAspectRatio] = make_pair((float)GAME_WIDTH / (float)GAME_HEIGHT, (float)m_outputWidth / (float)m_outputHeight); 
            gameAspectRatio > windowAspectRatio)
            scale = (float)m_outputHeight / (float)GAME_HEIGHT;
        else
            scale = (float)m_outputWidth / (float)GAME_WIDTH;

        float blurredWidth = (float)GAME_WIDTH * scale;
        float blurredHeight = (float)GAME_HEIGHT * scale;

        m_spriteBatch->Draw(m_bloomCamera->GetTexture()->GetShaderResourceView().Get(),
            Vector2(blurredWidth / 2.0f,
                blurredHeight / 2.0f),
            nullptr,
            Vector4::One,
            0.0f,
            Vector2(GAME_WIDTH / 2.0f,
                GAME_HEIGHT / 2.0f),
            Vector2::One * scale,
            SpriteEffects_None,
            0.0f);

        m_spriteBatch->End();

        m_bloomCamera->Bloom(m_pseudo3DCamera->GetTexture(),
            m_renderTargetView,
            m_depthStencilView);

        SetRenderTarget();

        m_spriteBatch->Begin(SpriteSortMode_Deferred,
            nullptr,
            m_states->PointClamp());

        m_bloomCamera->Present(m_spriteBatch);
        m_uiCamera->Present(m_spriteBatch);

        m_spriteBatch->End();

        m_d3dContext->PSSetShaderResources(0, 2, null);

        Present();
    }
    catch(exception e)
    {
        FileManager::GetInstance()->PushToLog("Error while rendering frame: " + to_string(m_timer.GetFrameCount()));
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::Black);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    SetRenderTarget();
}

void Game::SetRenderTarget()
{
    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    // If the device was reset we must completely reinitialize the renderer.
    if (HRESULT hr = m_swapChain->Present(1, 0); hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        OnDeviceLost();
    else
        DX::ThrowIfFailed(hr);
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

    if (m_bloomCamera != nullptr)
        m_bloomCamera->OnScreenResize(m_outputWidth,
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

void Game::OnDeviceLost(bool reset)
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

    if (reset)
    {
        CreateDevice();
        CreateResources();
    }
}

void Game::ChangeGameState(Game::GameState gameState)
{
    m_gameState = gameState;

    if (m_bloomCamera != nullptr)
    {
        if (m_gameState == Game::GameState::Playing)
            m_bloomCamera->SetBloomPreset(BloomCamera::BloomPresets::Default);
        else
            m_bloomCamera->SetBloomPreset(BloomCamera::BloomPresets::Blurry);
    }
}

void Game::Cleanup()
{
    FileManager::DeleteInstance();
    InputManager::DeleteInstance();
    
    m_mouse.reset();
    m_keyboard.reset();

    m_window = NULL;
    m_d3dDevice.Reset();
    m_d3dContext.Reset();
    m_swapChain.Reset();
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_spriteBatch.reset();
    m_states.reset();
    m_texture2DManager.reset();
    m_gameFontManager.reset();
    m_pseudo3DCamera.reset();
    m_bloomCamera.reset();
    m_collisionManager.reset();

    m_terrain.reset();
    m_player.reset();
    m_carTexture.reset();
    m_sunTexture.reset();
    m_uiCamera.reset();
    m_speedPointerImage.reset();
    m_distanceText.reset();
    m_scoreLabels.clear();
    m_bestScoreLabels.clear();
    m_livesLabel.reset();
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    FileManager::GetInstance()->PushToLog("Creating Device");
    try
    {
        UINT creationFlags = 0;

#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        static const D3D_FEATURE_LEVEL featureLevels[] =
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
                D3D11_MESSAGE_ID hide[] =
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
    }
    catch (exception e)
    {
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }

    InputManager::CreateInstance(m_keyboard, 
                                 m_mouse);

    m_spriteBatch = make_shared<SpriteBatch>(m_d3dContext.Get());
    m_states = make_unique<CommonStates>(m_d3dDevice.Get());

    CreateGameResources();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    FileManager::GetInstance()->PushToLog("Creating window size dependent resources");
    try
    {
        // Clear the previous window size specific context.
        ID3D11RenderTargetView* nullViews[] = { nullptr };
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
            if (HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0); hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
            {
                // If the device was removed for any reason, a new device and swap chain will need to be created.
                OnDeviceLost();

                // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
                // and correctly set up the new device.
                return;
            }
            else
                DX::ThrowIfFailed(hr);
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
    }
    catch (exception e)
    {
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }
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
    FileManager::GetInstance()->PushToLog("Creating Game resources");
    try
    {
        m_gameState        = Game::GameState::Playing;

        m_texture2DManager = make_shared<Texture2DManager>(m_d3dDevice,
                                                           "Resources/");

        m_gameFontManager  = make_shared<GameFontManager>(m_d3dDevice, 
                                                          "Resources/");

        m_pseudo3DCamera = make_shared<Pseudo3DCamera>(m_d3dDevice,
            m_d3dContext,
            this,
            GAME_WIDTH,
            GAME_HEIGHT,
            m_outputWidth,
            m_outputHeight,
            CAMERA_DEPTH);

        m_bloomCamera = make_shared<BloomCamera>(m_d3dDevice,
            m_d3dContext,
            this,
            GAME_WIDTH,
            GAME_HEIGHT,
            m_outputWidth,
            m_outputHeight);

        m_gameObjects = list<shared_ptr<GameObject> >();

        m_collisionManager = make_unique<CollisionManager>();

        m_sunTexture = m_texture2DManager->Load("Sun.png");

        // Add the terrain
        shared_ptr<GameObject> terrainObj = make_shared<GameObject>(this);

        shared_ptr<Terrain> terrain;

        terrain = make_shared<Terrain>(terrainObj.get(),
                                         m_texture2DManager,
                                         m_d3dDevice);

        terrainObj->AddComponent(terrain);
        m_gameObjects.push_back(terrainObj);
        m_terrain = terrainObj;

        // Add the player
        shared_ptr<GameObject> playerObj = make_shared<GameObject>(this);
        shared_ptr<Player> player = make_shared<Player>(playerObj.get());
        playerObj->AddComponent(player);
        m_player = playerObj;

        m_carTexture = m_texture2DManager->Load("Car.png");

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(playerObj.get(),
            m_carTexture);
        playerObj->AddComponent(spriteRenderer);

        m_gameObjects.push_back(playerObj);
    }
    catch (exception e)
    {
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }
    CreateUI();
}

void Game::CreateUI()
{
    FileManager::GetInstance()->PushToLog("Creating UI");
    try
    { 
        const float MARGIN = 10.0f;

        m_scoreLabels.clear();
        m_bestScoreLabels.clear();

        m_uiCamera = make_shared<UICamera>(m_d3dDevice, 
                                           m_d3dContext, 
                                           this, 
                                           GAME_WIDTH, 
                                           GAME_HEIGHT, 
                                           m_outputWidth, 
                                           m_outputHeight);

        for (int i = 0; i < (int)Game::GameState::Last; i++)
            m_uiLayers[i] = make_shared<UILayer>(Vector2::Zero, 
                                                 Vector2(GAME_WIDTH, 
                                                         GAME_HEIGHT));

        shared_ptr<Texture2D> replayTexture        = m_texture2DManager->Load("UI/Replay.png");
        shared_ptr<Texture2D> replayPressedTexture = m_texture2DManager->Load("UI/ReplayPressed.png");

        shared_ptr<Texture2D> quitTexture          = m_texture2DManager->Load("UI/Quit.png");
        shared_ptr<Texture2D> quitPressedTexture   = m_texture2DManager->Load("UI/QuitPressed.png");

        shared_ptr<Texture2D> resumeTexture        = m_texture2DManager->Load("UI/Resume.png");
        shared_ptr<Texture2D> resumePressedTexture = m_texture2DManager->Load("UI/ResumePressed.png");

        shared_ptr<Texture2D> pauseTexture         = m_texture2DManager->Load("UI/Pause.png");
        shared_ptr<Texture2D> pausePressedTexture  = m_texture2DManager->Load("UI/PausePressed.png");

        shared_ptr<Texture2D> speedometerTexture   = m_texture2DManager->Load("UI/Speedometer.png");
        shared_ptr<Texture2D> speedPointerTexture  = m_texture2DManager->Load("UI/SpeedPointer.png");
        shared_ptr<Texture2D> life17Texture        = m_texture2DManager->Load("UI/Life17.png");
        
        shared_ptr<GameFont> vcr17FontRed          = m_gameFontManager->Load("Fonts/VCR17.spritefont");
        vcr17FontRed->SetColor(Vector4(1, 0, 0.447, 1.0f));

        // create the ending screen

        shared_ptr<UIButton> endReplayButton = make_shared<UIButton>(Vector2((-replayTexture->GetWidth() / 2.0f) - MARGIN, 
                                                                             replayTexture->GetHeight() + 2.0f * MARGIN), 
                                                                     Vector2(replayTexture->GetWidth(),
                                                                             replayTexture->GetHeight()),
                                                                     replayTexture,
                                                                     replayPressedTexture,
                                                                     bind(&Game::OnReplayButtonReleased, this));

        m_uiLayers[(int)Game::GameState::End]->AddChild(endReplayButton);

        shared_ptr<UIButton> endQuitButton = make_shared<UIButton>(Vector2((quitTexture->GetWidth() / 2.0f) + MARGIN,
                                                                           quitTexture->GetHeight() + 2.0f * MARGIN),
                                                                   Vector2(quitTexture->GetWidth(),
                                                                   quitTexture->GetHeight()),
                                                                   quitTexture,
                                                                   quitPressedTexture,
                                                                   ExitGame);

        m_uiLayers[(int)Game::GameState::End]->AddChild(endQuitButton);

        CreateScoreUI(m_uiLayers[(int)Game::GameState::End], 
                      MARGIN);

        // create the pause screen
        shared_ptr<UIButton> pauseReplayButton = make_shared<UIButton>(Vector2((-replayTexture->GetWidth()) - MARGIN,
                                                                               replayTexture->GetHeight() + 2.0f * MARGIN),
                                                                       Vector2(replayTexture->GetWidth(),
                                                                               replayTexture->GetHeight()),
                                                                       replayTexture,
                                                                       replayPressedTexture,
                                                                       bind(&Game::OnReplayButtonReleased, 
                                                                            this));

        m_uiLayers[(int)Game::GameState::Pause]->AddChild(pauseReplayButton);

        shared_ptr<UIButton> pauseResumeButton = make_shared<UIButton>(Vector2(0.0f,
                                                                               replayTexture->GetHeight() + 2.0f * MARGIN),
                                                                       Vector2(resumeTexture->GetWidth(),
                                                                               resumeTexture->GetHeight()),
                                                                       resumeTexture,
                                                                       resumePressedTexture, 
                                                                       [&]() 
            {
                ChangeGameState(GameState::Playing);
            });

        m_uiLayers[(int)Game::GameState::Pause]->AddChild(pauseResumeButton);

        shared_ptr<UIButton> pauseQuitButton = make_shared<UIButton>(Vector2((quitTexture->GetWidth()) + MARGIN,
                                                                             quitTexture->GetHeight() + 2.0f * MARGIN),
                                                                     Vector2(quitTexture->GetWidth(),
                                                                             quitTexture->GetHeight()),
                                                                     quitTexture,
                                                                     quitPressedTexture,
                                                                     ExitGame);

        m_uiLayers[(int)Game::GameState::Pause]->AddChild(pauseQuitButton);

        CreateScoreUI(m_uiLayers[(int)Game::GameState::Pause],
                      MARGIN);

        // create the game screen

        shared_ptr<UIImage> speedometerImage = make_shared<UIImage>(speedometerTexture, 
                                                                    Vector2(GAME_WIDTH / 2.0f - speedometerTexture->GetWidth() / 2.0f,
                                                                            GAME_HEIGHT / 2.0f - speedometerTexture->GetHeight() / 2.0f + MARGIN),
                                                                    Vector2(speedometerTexture->GetWidth(),
                                                                            speedometerTexture->GetHeight()));

        m_uiLayers[(int)Game::GameState::Playing]->AddChild(speedometerImage);

        shared_ptr<UIImage> speedPointerImage = make_shared<UIImage>(speedPointerTexture,
                                                                     Vector2(GAME_WIDTH / 2.0f - speedPointerTexture->GetWidth() / 2.0f,
                                                                             GAME_HEIGHT / 2.0f - speedPointerTexture->GetHeight() / 2.0f + MARGIN),
                                                                     Vector2(speedPointerTexture->GetWidth(),
                                                                             speedPointerTexture->GetHeight()));

        m_uiLayers[(int)Game::GameState::Playing]->AddChild(speedPointerImage);
        m_speedPointerImage = speedPointerImage;

        shared_ptr<UIButton> pauseButton = make_shared<UIButton>(Vector2((-GAME_WIDTH / 2.0f) + MARGIN + (pauseTexture->GetWidth() / 2.0f),
                                                                         (-GAME_HEIGHT / 2.0f) + MARGIN + (pauseTexture->GetHeight() / 2.0f)),
                                                                 Vector2(pauseTexture->GetWidth(),
                                                                         pauseTexture->GetHeight()),
                                                                 pauseTexture,
                                                                 pausePressedTexture,
                                                                 [&]() 
            {
                ChangeGameState(Game::GameState::Pause);
            });

        m_uiLayers[(int)Game::GameState::Playing]->AddChild(pauseButton);

        m_distanceText = make_shared<UIText>(vcr17FontRed,
                                             Vector2(0.0f, -GAME_HEIGHT / 2.0f + 2.0f * MARGIN));
        m_distanceText->SetText("0");

        m_uiLayers[(int)Game::GameState::Playing]->AddChild(m_distanceText);

        shared_ptr<UIImage> lifeImage = make_shared<UIImage>(life17Texture,
                                                             Vector2((GAME_WIDTH / 2.0f) - MARGIN - (life17Texture->GetWidth() / 2.0f),
                                                                     (-GAME_HEIGHT / 2.0f) + MARGIN + (life17Texture->GetHeight() / 2.0f)),
                                                             Vector2(life17Texture->GetWidth(),
                                                                     life17Texture->GetHeight()));

        m_uiLayers[(int)Game::GameState::Playing]->AddChild(lifeImage);

        m_livesLabel = make_shared<UIText>(vcr17FontRed,
                                           Vector2(-2.0f * MARGIN, 0.0f));
        m_livesLabel->SetText("0");

        lifeImage->AddChild(m_livesLabel);
    }
    catch(exception e)
    {
        FileManager::GetInstance()->PushToLog(e.what());
        ExitGame();
    }
}

void Game::CreateScoreUI(shared_ptr<UILayer> layer, 
                         float               MARGIN)
{
    float currentUIHeight = 0.0f;
    shared_ptr<Texture2D> scoreBarTexture = m_texture2DManager->Load("UI/ScoreBar.png");

    shared_ptr<GameFont> vcr32FontBlack = m_gameFontManager->Load("Fonts/VCR32.spritefont");
    // make the vcr32FontBlack the color black.
    vcr32FontBlack->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
    shared_ptr<GameFont> vcr32FontRed = m_gameFontManager->Load("Fonts/VCR32.spritefont");
    vcr32FontRed->SetColor(Vector4(1, 0, 0.447, 1.0f));

    shared_ptr<UIImage> endHighScoreImage = make_shared<UIImage>(scoreBarTexture,
        Vector2(0.0f,
        (-GAME_HEIGHT / 2.0f) + currentUIHeight + (scoreBarTexture->GetHeight() / 2.0f)),
        Vector2(scoreBarTexture->GetWidth(),
            scoreBarTexture->GetHeight()));

    layer->AddChild(endHighScoreImage);
    shared_ptr<UIText> endBestScoreText = make_shared<UIText>(vcr32FontBlack,
        Vector2(0.0f,
            -MARGIN / 4.0f));
    endBestScoreText->SetText("BEST SCORE");
    endHighScoreImage->AddChild(dynamic_pointer_cast<UIElement>(endBestScoreText));

    currentUIHeight += scoreBarTexture->GetHeight();

    shared_ptr<UIText> endBestScore = make_shared<UIText>(vcr32FontRed,
        Vector2(0.0f,
        (-GAME_HEIGHT / 2.0f) + currentUIHeight + (scoreBarTexture->GetHeight() / 2.0f)));
    endBestScore->SetText("0");
    layer->AddChild(endBestScore);

    currentUIHeight += scoreBarTexture->GetHeight();

    shared_ptr<UIImage> endScoreImage = make_shared<UIImage>(scoreBarTexture,
        Vector2(0.0f,
        (-GAME_HEIGHT / 2.0f) + currentUIHeight + (scoreBarTexture->GetHeight() / 2.0f)),
        Vector2(scoreBarTexture->GetWidth(),
            scoreBarTexture->GetHeight()));

    layer->AddChild(endScoreImage);

    shared_ptr<UIText> endScoreText = make_shared<UIText>(vcr32FontBlack,
        Vector2(0.0f,
            0.0f));
    endScoreText->SetText("SCORE");
    endScoreImage->AddChild(dynamic_pointer_cast<UIElement>(endScoreText));

    currentUIHeight += scoreBarTexture->GetHeight();

    shared_ptr<UIText> endScore = make_shared<UIText>(vcr32FontRed,
        Vector2(0.0f,
        (-GAME_HEIGHT / 2.0f) + currentUIHeight + (scoreBarTexture->GetHeight() / 2.0f)));
    endScore->SetText("0");

    m_scoreLabels.push_back(endScore);
    m_bestScoreLabels.push_back(endBestScore);
    UpdateBestScoreLabels();

    layer->AddChild(endScore);
}

void Game::UpdateBestScoreLabels()
{
    int bestScore = FileManager::GetInstance()->ReadScore();
    for (int i = 0; i < m_bestScoreLabels.size(); i++)
        m_bestScoreLabels[i]->SetText(to_string(bestScore));
}

void Game::ReleaseGameResources()
{
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
    m_bloomCamera.reset();
    m_pseudo3DCamera.reset();
    m_texture2DManager.reset();
    m_gameFontManager.reset();

    m_speedPointerImage.reset();
    m_distanceText.reset();
    for (auto& scoreLabel : m_scoreLabels)
        scoreLabel.reset();

    for (auto& bestScoreLabel : m_bestScoreLabels)
        bestScoreLabel.reset();

    m_livesLabel.reset();

    for (int i = 0; i < (int)Game::GameState::Last; i++)
        if (m_uiLayers[i] != nullptr)
            m_uiLayers[i]->DeleteChildren();

    for (int i = 0; i < (int)Game::GameState::Last; i++)
        m_uiLayers[i].reset();
}

void Game::OnReplayButtonReleased()
{
    ChangeGameState(Game::GameState::Reseting);
}
