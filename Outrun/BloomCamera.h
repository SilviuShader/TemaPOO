#pragma once

// Implements the post-processing effect: Bloom
// most of the code is from: https://github.com/microsoft/DirectXTK/wiki/Writing-custom-shaders
// with slight adjustmens. 
// (I added heap memory allocation and assignment operator to the VS_BLUR_PARAMETERS class)
// The shaders used by this class are copied from that tutorial too.

class BloomCamera : public Camera
{
public:

    enum class BloomPresets
    {
        Default = 0,
        Soft,
        Desaturated,
        Saturated,
        Blurry,
        Subtle,
        None
    };

private:

    class VS_BLOOM_PARAMETERS
    {
    public:

        VS_BLOOM_PARAMETERS(float, 
                            float, 
                            float, 
                            float, 
                            float, 
                            float);

        inline float GetBlurAmount() const { return m_blurAmount; }

    private:

        float   m_bloomThreshold;
        float   m_blurAmount;
        float   m_bloomIntensity;
        float   m_baseIntensity;
        float   m_bloomSaturation;
        float   m_baseSaturation;
        uint8_t m_na[8];
    };

    struct VS_BLUR_PARAMETERS
    {
    private:

        static const size_t SAMPLE_COUNT = 15;
        static const int    DATA_SIZE    = sizeof(DirectX::XMFLOAT4) * 2 * SAMPLE_COUNT;

    public:

        static inline int GetDataSize() { return DATA_SIZE; }

    public:

        VS_BLUR_PARAMETERS();
        VS_BLUR_PARAMETERS(const VS_BLUR_PARAMETERS&);
        ~VS_BLUR_PARAMETERS();



        void                SetBlurEffectParameters(float, 
                                                    float,
                                                    const BloomCamera::VS_BLOOM_PARAMETERS&);
        char*               GetData();

        VS_BLUR_PARAMETERS& operator=(const VS_BLUR_PARAMETERS&);

    private:

        float ComputeGaussian(float, 
                              float);
    private:

        char*              m_data;

        DirectX::XMFLOAT4* m_sampleOffsets;
        DirectX::XMFLOAT4* m_sampleWeights;
    };

private:

    const int BLOOM_BEGIN_PIPELINE_ID = 2;

public:

	BloomCamera(Microsoft::WRL::ComPtr<ID3D11Device>,
                Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
                std::shared_ptr<Game>,
                int,
                int,
                int,
                int);

    ~BloomCamera();

           void                      SetBloomPreset(BloomCamera::BloomPresets);

           void                      Bloom(std::shared_ptr<Texture2D>,
                                           Microsoft::WRL::ComPtr<ID3D11RenderTargetView>,
                                           Microsoft::WRL::ComPtr<ID3D11DepthStencilView>);

    inline BloomCamera::BloomPresets GetBloomPreset() const { return m_bloomPreset; }

private:

    void SetBloomParameters();

private:

    static const BloomCamera::VS_BLOOM_PARAMETERS g_bloomPresets[];

private:

    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomExtractPS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_bloomCombinePS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianBlurPS;

    Microsoft::WRL::ComPtr<ID3D11Buffer>      m_bloomParams;
    Microsoft::WRL::ComPtr<ID3D11Buffer>      m_blurParamsWidth;
    Microsoft::WRL::ComPtr<ID3D11Buffer>      m_blurParamsHeight;

    std::shared_ptr<RenderTexture>            m_renderTexture1;
    std::shared_ptr<RenderTexture>            m_renderTexture2;
    RECT                                      m_bloomRect;

    BloomCamera::BloomPresets                 m_bloomPreset;
};