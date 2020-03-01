#include "pch.h"
#include "ContentManager.h"

using namespace std;

ContentManager::ContentManager(ID3D11Device* d3dDevice, string path) :
	m_d3dDevice(d3dDevice),
	m_path(path)
{
}

ContentManager::ContentManager(const ContentManager& other)
{
}

ContentManager::~ContentManager()
{
}