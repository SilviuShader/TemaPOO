#pragma once

class UIImage : public UIElement
{
public:

    UIImage(std::shared_ptr<Texture2D>,
            DirectX::SimpleMath::Vector2,
            DirectX::SimpleMath::Vector2);

    ~UIImage();

    inline void SetRotation(float rotation) { m_rotation = rotation; }

protected:

    void ElementRender(std::shared_ptr<UICamera>) override;

private:

    std::shared_ptr<Texture2D> m_texture;
    float                      m_rotation;
};