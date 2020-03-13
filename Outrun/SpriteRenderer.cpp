#include "pch.h"
#include "SpriteRenderer.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

SpriteRenderer::SpriteRenderer(shared_ptr<GameObject> parent, 
                               shared_ptr<Texture2D>  texture) :
    GameComponent(parent),
    m_texture(texture)
{
}

SpriteRenderer::~SpriteRenderer()
{
    m_texture.reset();
}

void SpriteRenderer::Render()
{
    shared_ptr<Transform>      transform = m_parent->GetTransform();
    shared_ptr<Game>           game      = m_parent->GetGame();

    shared_ptr<Terrain>        terrain   = game->GetTerrain();
    shared_ptr<Player>         player    = game->GetPlayer();
    shared_ptr<Pseudo3DCamera> camera    = game->GetPseudo3DCamera();

    float scale = transform->GetScale() * ((float)camera->GetHeight() / BASE_TEXTURES_RESOLUTION);

    // That's a big-ass formula.. unfortunately I can't really make it look prettier.
    float normalizedDifference = (terrain->GetRoadX(camera->GetLine(transform->GetPositionZ())) + terrain->GetAccumulatedTranslation() - player->GetPositionX() + transform->GetPositionX()) * transform->GetScale();
    float rawDifference        = normalizedDifference * camera->GetWidth() / 2.0f;

    if (transform->GetScale() > 0.0f)
    {
        camera->Begin2D();

        camera->DrawSprite(m_texture,
                           Vector2(rawDifference,
                                   (camera->GetLine(transform->GetPositionZ()) - (camera->GetHeight() / 2.0f)) - ((m_texture->GetHeight() / 2.0f) * scale)),
                           nullptr,
                           0.0f,
                           Vector2(scale,
                                   scale));
    }
}
