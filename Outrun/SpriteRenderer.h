#pragma once

class SpriteRenderer : public GameComponent
{
public:
    
    SpriteRenderer(GameObject*, Texture2D*);
    ~SpriteRenderer();

    void Update(float)           override;
    void Render(Pseudo3DCamera*) override;

private:

    Texture2D* m_texture;
};
