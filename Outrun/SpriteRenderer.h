#pragma once

class SpriteRenderer : public GameComponent
{
private:

    const float BASE_TEXTURES_RESOLUTION = 240;

public:
    
    SpriteRenderer(GameObject*, 
                   std::shared_ptr<Texture2D>);
    ~SpriteRenderer();

    void  Update(float) override { };
    void  Render()      override;

    float GetSpriteScaleFactor();
    RECT  GetSpriteRect();

private:

    std::shared_ptr<Texture2D> m_texture;
};
