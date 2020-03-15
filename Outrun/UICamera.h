#pragma once

class UICamera : public Camera
{
public:

    UICamera(Microsoft::WRL::ComPtr<ID3D11Device>,
             Microsoft::WRL::ComPtr<ID3D11DeviceContext>,
             std::shared_ptr<Game>,
             int,
             int,
             int,
             int);
    
    ~UICamera();
};