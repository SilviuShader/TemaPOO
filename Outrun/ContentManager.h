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

    if (typeid(T).hash_code() == typeid(Texture2D).hash_code() || 
        typeid(T).hash_code() == typeid(GameFont).hash_code())
        result = std::make_shared<T>(m_d3dDevice, fullPath);
    
    return result;
}
