#pragma once

class UIText : public UIElement
{
public:

    UIText(std::shared_ptr<GameFont>,
           DirectX::SimpleMath::Vector2);

    ~UIText();
    
    void SetText(std::string);

protected:

    void ElementRender(std::shared_ptr<UICamera>) override;

private:

    std::shared_ptr<GameFont> m_font;
    std::string               m_message;
};