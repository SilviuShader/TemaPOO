#pragma once

class BloomCamera : public Camera
{
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

    void Bloom(std::shared_ptr<Texture2D>,
               Microsoft::WRL::ComPtr<ID3D11RenderTargetView>,
               Microsoft::WRL::ComPtr<ID3D11DepthStencilView>);

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
};