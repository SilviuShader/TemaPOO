#pragma once

class UIButton : public UIElement
{
public:

    UIButton(DirectX::SimpleMath::Vector2,
             DirectX::SimpleMath::Vector2,
             std::shared_ptr<Texture2D>,
             std::shared_ptr<Texture2D>   = nullptr,
             std::function<void _cdecl()> = nullptr,
             std::function<void _cdecl()> = nullptr);

    ~UIButton();

protected:

    void ElementUpdate(std::shared_ptr<UICamera>) override;
    void ElementRender(std::shared_ptr<UICamera>) override;

private:

    std::shared_ptr<Texture2D>   m_activeTexture;
    std::shared_ptr<Texture2D>   m_pressedTexture;
    std::function<void _cdecl()> m_onPressedFunction;
    std::function<void _cdecl()> m_onReleasedFunction;
};