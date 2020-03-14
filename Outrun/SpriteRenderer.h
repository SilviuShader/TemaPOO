#pragma once

class SpriteRenderer : public GameComponent
{
private:

    const float BASE_TEXTURES_RESOLUTION = 300;

public:
    
    SpriteRenderer(std::shared_ptr<GameObject>, 
                   std::shared_ptr<Texture2D>);
    ~SpriteRenderer();

    inline void  Update(float) override { };
           void  Render()      override;

           float GetSpriteScaleFactor();
           RECT  GetSpriteRect();

private:

    std::shared_ptr<Texture2D> m_texture;
};
