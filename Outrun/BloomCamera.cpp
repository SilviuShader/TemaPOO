#include "pch.h"

using namespace std;

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace
{
    struct VS_BLOOM_PARAMETERS
    {
        float bloomThreshold;
        float blurAmount;
        float bloomIntensity;
        float baseIntensity;
        float bloomSaturation;
        float baseSaturation;
        uint8_t na[8];
    };

    static_assert(!(sizeof(VS_BLOOM_PARAMETERS) % 16),
        "VS_BLOOM_PARAMETERS needs to be 16 bytes aligned");

    struct VS_BLUR_PARAMETERS
    {
        static const size_t SAMPLE_COUNT = 15;

        XMFLOAT4 sampleOffsets[SAMPLE_COUNT];
        XMFLOAT4 sampleWeights[SAMPLE_COUNT];

        void SetBlurEffectParameters(float dx, float dy,
            const VS_BLOOM_PARAMETERS& params)
        {
            sampleWeights[0].x = ComputeGaussian(0, params.blurAmount);
            sampleOffsets[0].x = sampleOffsets[0].y = 0.f;

            float totalWeights = sampleWeights[0].x;

            // Add pairs of additional sample taps, positioned
            // along a line in both directions from the center.
            for (size_t i = 0; i < SAMPLE_COUNT / 2; i++)
            {
                // Store weights for the positive and negative taps.
                float weight = ComputeGaussian(float(i + 1.f), params.blurAmount);

                sampleWeights[i * 2 + 1].x = weight;
                sampleWeights[i * 2 + 2].x = weight;

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
                sampleOffsets[i * 2 + 1].x = delta.x;
                sampleOffsets[i * 2 + 1].y = delta.y;
                sampleOffsets[i * 2 + 2].x = -delta.x;
                sampleOffsets[i * 2 + 2].y = -delta.y;
            }

            for (size_t i = 0; i < SAMPLE_COUNT; i++)
            {
                sampleWeights[i].x /= totalWeights;
            }
        }

    private:
        float ComputeGaussian(float n, float theta)
        {
            return (float)((1.0 / sqrtf(2 * XM_PI * theta))
                * expf(-(n * n) / (2 * theta * theta)));
        }
    };

    static_assert(!(sizeof(VS_BLUR_PARAMETERS) % 16),
        "VS_BLUR_PARAMETERS needs to be 16 bytes aligned");

    enum BloomPresets
    {
        Default = 0,
        Soft,
        Desaturated,
        Saturated,
        Blurry,
        Subtle,
        None
    };

    BloomPresets g_Bloom = Default;

    static const VS_BLOOM_PARAMETERS g_BloomPresets[] =
    {
        //Thresh  Blur Bloom  Base  BloomSat BaseSat
        { 0.25f,  4,   1.25f, 1,    1,       1 }, // Default
        { 0,      3,   1,     1,    1,       1 }, // Soft
        { 0.5f,   8,   2,     1,    0,       1 }, // Desaturated
        { 0.25f,  4,   2,     1,    2,       1 }, // Saturated
        { 0,      2,   1,     0.1f, 1,       1 }, // Blurry
        { 0.5f,   2,   1,     1,    1,       1 }, // Subtle
        { 0.25f,  4,   1.25f, 1,    1,       1 }, // None
    };
}

BloomCamera::BloomCamera(ComPtr<ID3D11Device>        d3dDevice, 
                         ComPtr<ID3D11DeviceContext> d3dContext, 
                         shared_ptr<Game>            game, 
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
    auto blob = DX::ReadData(L"BloomExtract.cso");
    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomExtractPS.ReleaseAndGetAddressOf()));

    blob = DX::ReadData(L"BloomCombine.cso");
    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_bloomCombinePS.ReleaseAndGetAddressOf()));

    blob = DX::ReadData(L"GaussianBlur.cso");
    DX::ThrowIfFailed(d3dDevice->CreatePixelShader(blob.data(), blob.size(),
        nullptr, m_gaussianBlurPS.ReleaseAndGetAddressOf()));

    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLOOM_PARAMETERS),
            D3D11_BIND_CONSTANT_BUFFER);
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &g_BloomPresets[g_Bloom];
        initData.SysMemPitch = sizeof(VS_BLOOM_PARAMETERS);
        DX::ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, &initData,
            m_bloomParams.ReleaseAndGetAddressOf()));
    }

    {
        CD3D11_BUFFER_DESC cbDesc(sizeof(VS_BLUR_PARAMETERS),
            D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsWidth.ReleaseAndGetAddressOf()));
        DX::ThrowIfFailed(d3dDevice->CreateBuffer(&cbDesc, nullptr,
            m_blurParamsHeight.ReleaseAndGetAddressOf()));
    }

    VS_BLUR_PARAMETERS blurData;
    blurData.SetBlurEffectParameters(1.f / (width / 2), 0,
        g_BloomPresets[g_Bloom]);
    m_d3dContext->UpdateSubresource(m_blurParamsWidth.Get(), 0, nullptr,
        &blurData, sizeof(VS_BLUR_PARAMETERS), 0);

    blurData.SetBlurEffectParameters(0, 1.f / (height / 2),
        g_BloomPresets[g_Bloom]);
    m_d3dContext->UpdateSubresource(m_blurParamsHeight.Get(), 0, nullptr,
        &blurData, sizeof(VS_BLUR_PARAMETERS), 0);

    m_renderTexture1 = make_shared<RenderTexture>(d3dDevice, 
                                                  width / 2.0f, 
                                                  height / 2.0f);

    m_renderTexture2 = make_shared<RenderTexture>(d3dDevice,
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

void BloomCamera::Bloom(shared_ptr<Texture2D>          source,
                        ComPtr<ID3D11RenderTargetView> renderTargetView,
                        ComPtr<ID3D11DepthStencilView> depthStencilView)
{
    if (g_Bloom == None)
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

        m_spriteBatch->Draw(source->GetShaderResourceView(), m_bloomRect);
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
