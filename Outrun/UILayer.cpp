#include "pch.h"

using namespace DirectX::SimpleMath;

UILayer::UILayer(Vector2 relativePosition, 
                 Vector2 size)
{
    SetRelativePosition(relativePosition);
    SetSize(size);
}

UILayer::~UILayer()
{
}
