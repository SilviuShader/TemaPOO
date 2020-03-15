#include "pch.h"

using namespace std;
using namespace Microsoft::WRL;

UICamera::UICamera(ComPtr<ID3D11Device>        d3dDevice, 
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
}

UICamera::~UICamera()
{
}
