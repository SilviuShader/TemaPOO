#include "pch.h"

using namespace std;

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

using namespace DX;

const BloomCamera::VS_BLOOM_PARAMETERS BloomCamera::g_bloomPresets[] =
{
    //Thresh  Blur Bloom  Base  BloomSat BaseSat
    { 0.25f,  4,   1.25f, 1,    1,       1 }, // Default
    { 0,      3,   1,     1,    1,       1 }, // Soft
    { 0.5f,   8,   2,     1,    0,       1 }, // Desaturated
    { 0.25f,  4,   2,     1,    2,       0 }, // Saturated
    { 0,      2,   1,     0.1f, 1,       1 }, // Blurry
    { 0.5f,   2,   1,     1,    1,       1 }, // Subtle
    { 0.25f,  4,   1.25f, 1,    1,       1 }, // None
};

BloomCamera::VS_BLOOM_PARAMETERS::VS_BLOOM_PARAMETERS(float bloomThreshold, 
                                                      float blurAmount, 
                                                      float bloomIntensity, 
                                                      float baseIntensity, 
                                                      float bloomSaturation, 
                                                      float baseSaturation) :
    m_bloomThreshold(bloomThreshold),
    m_blurAmount(blurAmount),
    m_bloomIntensity(bloomIntensity),
    m_baseIntensity(baseIntensity),
    m_bloomSaturation(bloomSaturation),
    m_baseSaturation(baseSaturation)
{
}

BloomCamera::VS_BLUR_PARAMETERS::VS_BLUR_PARAMETERS()
{
    static_assert(!(DATA_SIZE % 16), 
                  "Data hold by the VS_BLUR_PARAMETERS class  needs to be 16 bytes aligned");

    m_data = new char[GetDataSize()];
    memset(m_data, 0, GetDataSize());

    m_sampleOffsets = new XMFLOAT4[SAMPLE_COUNT];
    memset(m_sampleOffsets, 0, sizeof(XMFLOAT4) * SAMPLE_COUNT);

    m_sampleWeights = new XMFLOAT4[SAMPLE_COUNT];
    memset(m_sampleWeights, 0, sizeof(XMFLOAT4) * SAMPLE_COUNT);
}

BloomCamera::VS_BLUR_PARAMETERS::VS_BLUR_PARAMETERS(const VS_BLUR_PARAMETERS& other)
{
    static_assert(!(DATA_SIZE % 16),
                  "Data hold by the VS_BLUR_PARAMETERS class  needs to be 16 bytes aligned");

    m_data = new char[GetDataSize()];
    memcpy(m_data, other.m_data, GetDataSize());

    m_sampleOffsets = new XMFLOAT4[SAMPLE_COUNT];
    memcpy(m_sampleOffsets, other.m_sampleOffsets, sizeof(XMFLOAT4) * SAMPLE_COUNT);

    m_sampleWeights = new XMFLOAT4[SAMPLE_COUNT];
    memcpy(m_sampleWeights, other.m_sampleWeights, sizeof(XMFLOAT4) * SAMPLE_COUNT);
}

BloomCamera::VS_BLUR_PARAMETERS::~VS_BLUR_PARAMETERS()
{
    if (m_sampleWeights)
    {
        delete[] m_sampleWeights;
        m_sampleWeights = NULL;
    }

    if (m_sampleOffsets)
    {
        delete[] m_sampleOffsets;
        m_sampleOffsets = NULL;
    }

    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
}

void BloomCamera::VS_BLUR_PARAMETERS::SetBlurEffectParameters(float dx, float dy, const BloomCamera::VS_BLOOM_PARAMETERS& params)
{
    m_sampleWeights[0].x = ComputeGaussian(0, params.GetBlurAmount());
    m_sampleOffsets[0].x = m_sampleOffsets[0].y = 0.f;

    float totalWeights = m_sampleWeights[0].x;

    // Add pairs of additional sample taps, positioned
    // along a line in both directions from the center.
    for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
    {
        // Store weights for the positive and negative taps.
        float weight = ComputeGaussian(float(i + 1.f), params.GetBlurAmount());

        m_sampleWeights[i * 2 + 1].x = weight;
        m_sampleWeights[i * 2 + 2].x = weight;

        totalWeights += weight * 2;

        // To get the maximum amount of blurring from a limited number of
        // pixel shader samples, we take advantage of the bilinear filtering
        // hardware inside the texture fetch unit. If we position our texture
        // coordinates exactly halfway between two texels, the filtering unit
        // will average them for us, giving two samples for the price of one.
        // This allows us to step in units of two texels per sample, rather
        // than just one at a time. The 1.5 offset kicks things off by
        // positioning us nicely in between two texels.
        float sampleOffset = float(i) * 2.f + 1.5f;

        Vector2 delta = Vector2(dx, dy) * sampleOffset;

        // Store texture coordinate offsets for the positive and negative taps.
        m_sampleOffsets[i * 2 + 1].x = delta.x;
        m_sampleOffsets[i * 2 + 1].y = delta.y;
        m_sampleOffsets[i * 2 + 2].x = -delta.x;
        m_sampleOffsets[i * 2 + 2].y = -delta.y;
    }

    for (size_t i = 0; i < SAMPLE_COUNT; i++)
        m_sampleWeights[i].x /= totalWeights;
}

char* BloomCamera::VS_BLUR_PARAMETERS::GetData()
{
    memcpy(m_data, m_sampleOffsets, sizeof(XMFLOAT4) * SAMPLE_COUNT);
    memcpy(m_data + (sizeof(XMFLOAT4) * SAMPLE_COUNT), m_sampleWeights, sizeof(XMFLOAT4) * SAMPLE_COUNT);
    return m_data;
}

BloomCamera::VS_BLUR_PARAMETERS& BloomCamera::VS_BLUR_PARAMETERS::operator=(const VS_BLUR_PARAMETERS& rhs)
{
    memcpy(m_data, rhs.m_data, GetDataSize());
    memcpy(m_sampleOffsets, rhs.m_sampleOffsets, sizeof(XMFLOAT4) * SAMPLE_COUNT);
    memcpy(m_sampleWeights, rhs.m_sampleWeights, sizeof(XMFLOAT4) * SAMPLE_COUNT);

    return *this;
}

float BloomCamera::VS_BLUR_PARAMETERS::ComputeGaussian(float n, float theta)
{
    return (float)((1.0 / sqrtf(2 * XM_PI * theta))
        * expf(-(n * n) / (2 * theta * theta)));
}

BloomCamera::BloomCamera(ComPtr<ID3D11Device>        d3dDevice, 
                         ComPtr<ID3D11DeviceContext> d3dContext, 
                         Game*                       game, 
                         int                         width, 
                         int                         height, 
                         int                         screenWidth, 
                         int                         screenHeight) :
    Camera(d3dDevice, 
           d3dContext, 
           game, 
           width, 
           height, 
           screenWidth, 
           screenHeight)
{
    static_assert(!(sizeof(BloomCamera::VS_BLOOM_PARAMETERS) % 16),
        "VS_BLOOM_PARAMETERS needs to be 16 bytes aligned");

    FileManager::GetInstance()->PushToLog("Creating BloomCamera");

    m_bloomPreset = BloomCamera::BloomPresets::Default;

    auto blob = ReadData(L"BloomExtract.cso");
    ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf()));

    blob = ReadData(L"BloomCombine.cso");
    ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf()));

    blob = ReadData(L"GaussianBlur.cso");
    ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf()));

    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLOOM_PARAMETERS),
            D3D11_BIND_CONSTANT_BUFFER);
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &g_bloomPresets[(int)m_bloomPreset];
        initData.SysMemPitch = sizeof(VS_BLOOM_PARAMETERS);
        ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, &initData,
            m_bloomParams.ReleaseAndGetAddressOf()));
    }

    {
        CD3D11_BUFFER_DESC cbDesc(VS_BLUR_PARAMETERS::GetDataSize(),
            D3D11_BIND_CONSTANT_BUFFER);
        ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsWidth.ReleaseAndGetAddressOf()));
        ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsHeight.ReleaseAndGetAddressOf()));
    }

    SetBloomParameters();

    m_renderTexture1 = make_unique<RenderTexture>(d3dDevice, 
                                                  width / 2.0f, 
                                                  height / 2.0f);

    m_renderTexture2 = make_unique<RenderTexture>(d3dDevice,
                                                  width / 2.0f,
                                                  height / 2.f);

    m_bloomRect.left = 0;
    m_bloomRect.top = 0;
    m_bloomRect.right = width / 2;
    m_bloomRect.bottom = height / 2;
}

BloomCamera::~BloomCamera()
{
    m_renderTexture2.reset();
    m_renderTexture1.reset();

    m_bloomExtractPS.Reset();
    m_bloomCombinePS.Reset();
    m_gaussianBlurPS.Reset();

    m_bloomParams.Reset();
    m_blurParamsWidth.Reset();
    m_blurParamsHeight.Reset();
}

void BloomCamera::SetBloomPreset(BloomCamera::BloomPresets bloomPreset)
{
    m_bloomPreset = bloomPreset;
    SetBloomParameters();
}

void BloomCamera::Bloom(shared_ptr<Texture2D>          source,
                        ComPtr<ID3D11RenderTargetView> renderTargetView,
                        ComPtr<ID3D11DepthStencilView> depthStencilView)
{
    if (m_bloomPreset == BloomCamera::BloomPresets::None)
    {
        Begin(Vector4::Zero);
        DrawSprite(source, Vector2::Zero, nullptr, 0.0f, Vector2::One);
        End(1,
            renderTargetView,
            depthStencilView);
    }
    else
    {
        ID3D11ShaderResourceView* const null[] = { nullptr, nullptr };
     
        // downsample
        m_renderTexture1->SetRenderTarget(m_d3dContext);
        m_renderTexture1->ClearRenderTarget(m_d3dContext,
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f);
        End2D();

        m_spriteBatch->Begin(SpriteSortMode_Immediate, 
                             nullptr,
                             nullptr,
                             nullptr,
                             nullptr,
                             [=]()
            {
                m_d3dContext->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
                m_d3dContext->PSSetShader(m_bloomExtractPS.Get(), nullptr, 0);
            });

        m_spriteBatch->Draw(source->GetShaderResourceView().Get(), m_bloomRect);
        m_spriteBatch->End();

        End(1,
            renderTargetView,
            depthStencilView);

        // blur horizontal
        m_renderTexture2->SetRenderTarget(m_d3dContext);
        m_renderTexture2->ClearRenderTarget(m_d3dContext,
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f);
        End2D();

        m_spriteBatch->Begin(SpriteSortMode_Immediate,
                             nullptr,
                             nullptr,
                             nullptr,
                             nullptr,
            [=]()
            {
                m_d3dContext->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                m_d3dContext->PSSetConstantBuffers(0, 1,
                    m_blurParamsWidth.GetAddressOf()); 
            });

        m_spriteBatch->Draw(m_renderTexture1->GetShaderResourceView().Get(), m_bloomRect);
        m_spriteBatch->End();

        End(1,
            renderTargetView,
            depthStencilView);

        m_d3dContext->PSSetShaderResources(0, 2, null);

        // blur vertical

        m_renderTexture1->SetRenderTarget(m_d3dContext);
        m_renderTexture1->ClearRenderTarget(m_d3dContext,
                                            0.0f,
                                            0.0f,
                                            0.0f,
                                            0.0f);
        End2D();

        m_spriteBatch->Begin(SpriteSortMode_Immediate,
                             nullptr,
                             nullptr,
                             nullptr,
                             nullptr,
            [=]()
            {
                m_d3dContext->PSSetShader(m_gaussianBlurPS.Get(), nullptr, 0);
                m_d3dContext->PSSetConstantBuffers(0, 1,
                    m_blurParamsHeight.GetAddressOf());
            });

        m_spriteBatch->Draw(m_renderTexture2->GetShaderResourceView().Get(), m_bloomRect);
        m_spriteBatch->End();

        End(1,
            renderTargetView,
            depthStencilView);

        
        Begin(Vector4::Zero);
        shared_ptr<CameraBeginFunction> begFunction = make_shared<CameraBeginFunction>([=]() 
            {
                m_d3dContext->PSSetShader(m_bloomCombinePS.Get(), nullptr, 0);
                m_d3dContext->PSSetShaderResources(1, 1, m_renderTexture1->GetShaderResourceView().GetAddressOf());
                m_d3dContext->PSSetConstantBuffers(0, 1, m_bloomParams.GetAddressOf());
            }, 
            BLOOM_BEGIN_PIPELINE_ID);

        Begin2D(begFunction);
        DrawSprite(source, Vector2::Zero, nullptr, 0.0f, Vector2::One);
        End(1,
            renderTargetView,
            depthStencilView);
    }
}

void BloomCamera::SetBloomParameters()
{
    m_d3dContext->UpdateSubresource(m_bloomParams.Get(), 0, nullptr,
        &g_bloomPresets[(int)m_bloomPreset], sizeof(VS_BLOOM_PARAMETERS), 0);

    VS_BLUR_PARAMETERS blurData;
    blurData.SetBlurEffectParameters(1.f / (m_width / 2), 0,
        g_bloomPresets[(int)m_bloomPreset]);
    m_d3dContext->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr,
                                    blurData.GetData(), VS_BLUR_PARAMETERS::GetDataSize(), 0);

    VS_BLUR_PARAMETERS blurData2;
    blurData2 = blurData;

    blurData2.SetBlurEffectParameters(0, 1.f / (m_height / 2),
        g_bloomPresets[(int)m_bloomPreset]);
    m_d3dContext->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr,
                                    blurData2.GetData(), VS_BLUR_PARAMETERS::GetDataSize(), 0);
}
