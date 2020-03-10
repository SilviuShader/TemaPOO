#include "pch.h"
#include "SpriteRenderer.h"

using namespace std;

using namespace DirectX;
using namespace DirectX::SimpleMath;

SpriteRenderer::SpriteRenderer(GameObject* parent, Texture2D* texture) :
    GameComponent(parent),
    m_texture(texture)
{
}

SpriteRenderer::~SpriteRenderer()
{
    // not deleting it here because the texture id not created inside this class
    // whoever created it has to delete it himself... it was probably created and stored
    // in some smart pointers anyways.
    m_texture = NULL;
}

void SpriteRenderer::Update(float deltaTime)
{
}

void SpriteRenderer::Render(Pseudo3DCamera* camera)
{
    shared_ptr<Transform> transform = m_parent->GetTransform();
    Terrain*              terrain   = m_parent->GetGame()->GetTerrain();
    Player*               player    = m_parent->GetGame()->GetPlayer();

    float normalizedDifference = (terrain->GetRoadX(transform->GetLine(), camera->GetZ(transform->GetLine())) + terrain->GetAccumulatedTranslation(camera->GetHeight() - 1) - player->GetPositionX() + transform->GetPositionX()) * (transform->GetScale());
    float rawDifference = normalizedDifference * camera->GetWidth() / 2.0f;

    if (transform->GetScale() > 0.0f)
    {
        camera->Begin2D();

        camera->DrawSprite(m_texture,
                           Vector2(rawDifference,
                                   (transform->GetLine() - camera->GetHeight() / 2.0f) - ((m_texture->GetHeight() / 2.0f) * transform->GetScale())),
                           nullptr,
                           0.0f,
                           Vector2(transform->GetScale(),
                                   transform->GetScale()));
    }
}
