#pragma once

class GameFont
{
public:

    GameFont(Microsoft::WRL::ComPtr<ID3D11Device>, 
             std::string);

    ~GameFont();

    inline void SetColor(DirectX::SimpleMath::Vector4 color)          { m_color = color; }

    inline std::shared_ptr<DirectX::SpriteFont> GetSpriteFont() const { return m_font;   }
    inline DirectX::SimpleMath::Vector4         GetColor()      const { return m_color;  }

private:

    std::shared_ptr<DirectX::SpriteFont> m_font;
    DirectX::SimpleMath::Vector4         m_color;
};


