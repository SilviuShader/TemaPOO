#include "pch.h"

using namespace std;

using namespace Microsoft::WRL;

using namespace DirectX;
using namespace DirectX::SimpleMath;

GameFont::GameFont(ComPtr<ID3D11Device> d3dDevice, 
                   string               filename) :
    m_color(Vector4::One)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t> > converter;
    m_font = make_shared<SpriteFont>(d3dDevice.Get(), 
                                     converter.from_bytes(filename).c_str());
}

GameFont::~GameFont()
{
    m_font.reset();
}
