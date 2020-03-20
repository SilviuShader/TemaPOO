#pragma once

class ContentManager
{
public:
    
    ContentManager(Microsoft::WRL::ComPtr<ID3D11Device>, 
                   std::string);
    ContentManager(const ContentManager&);
    ~ContentManager();

    template <class T> 
    std::shared_ptr<T> Load(std::string);

private:

    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    std::string                          m_path;
};

template<class T>
inline std::shared_ptr<T> ContentManager::Load(std::string filename)
{
    std::shared_ptr<T> result = NULL;
    std::string fullPath = m_path + filename;

    FileManager::GetInstance()->PushToLog("Loading: \"" + fullPath + "\"");

    if constexpr (std::is_same<T, Texture2D>())
        result = std::make_shared<T>(m_d3dDevice, fullPath);
    else if constexpr (std::is_same<T, GameFont>())
        result = std::make_shared<T>(m_d3dDevice, fullPath);
    else
        throw std::exception((std::string("The type \"") + (std::string)typeid(T).name() + std::string("\" cannot be loaded by ContentManager")).c_str());
    
    return result;
}
