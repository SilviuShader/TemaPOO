#pragma once

template <class T> 
class ContentManager
{
public:
    
    ContentManager(Microsoft::WRL::ComPtr<ID3D11Device>, 
                   std::string);
    ~ContentManager();

    std::shared_ptr<T> Load(std::string);

private:

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    std::string                          m_path;
};

template<class T>
ContentManager<T>::ContentManager(Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice,
                                  std::string                          path) :
    m_d3dDevice(d3dDevice),
    m_path(path)
{
}

template<class T>
ContentManager<T>::~ContentManager()
{
    m_d3dDevice.Reset();
}

template<class T>
std::shared_ptr<T> ContentManager<T>::Load(std::string filename)
{
    std::shared_ptr<T> result = NULL;
    std::string fullPath = m_path + filename;

    FileManager::GetInstance()->PushToLog("Loading: \"" + fullPath + "\"");

    if constexpr (std::is_same<T, Texture2D>())
        result = make_shared<T>(m_d3dDevice, fullPath);
    else if constexpr (std::is_same<T, GameFont>())
        result = std::make_shared<T>(m_d3dDevice, fullPath);
    else
        throw std::exception((std::string("The type \"") + (std::string)typeid(T).name() + std::string("\" cannot be loaded by ContentManager")).c_str());

    return result;
}

typedef ContentManager<Texture2D> Texture2DManager;
typedef ContentManager<GameFont>  GameFontManager;