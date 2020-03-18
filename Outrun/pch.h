//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <map>

#include <winsdkver.h>
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>
#include <list>

#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "ReadData.h"

#include "Utils.h"
#include "InputManager.h"
#include "FileManager.h"
#include "RenderTexture.h"
#include "Texture2D.h"
#include "ContentManager.h"
#include "Camera.h"
#include "BloomCamera.h"
#include "GameFont.h"
#include "UICamera.h"
#include "UIElement.h"
#include "UILayer.h"
#include "UIText.h"
#include "UIImage.h"
#include "UIButton.h"
#include "GameComponent.h"
#include "ObjectTranslator.h"
#include "Killer.h"
#include "ObjectsGenerator.h"
#include "Terrain.h"
#include "SpriteRenderer.h"
#include "Transform.h"
#include "Player.h"
#include "Pseudo3DCamera.h"
#include "GameObject.h"
#include "CollisionManager.h"
#include "Game.h"

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}
