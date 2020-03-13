#include "pch.h"
#include "ContentManager.h"

using namespace std;
using namespace Microsoft::WRL;

ContentManager::ContentManager(ComPtr<ID3D11Device> d3dDevice, 
	                           string               path) :
	m_d3dDevice(d3dDevice),
	m_path(path)
{
}

ContentManager::ContentManager(const ContentManager& other)
{
}

ContentManager::~ContentManager()
{
	m_d3dDevice.Reset();
}