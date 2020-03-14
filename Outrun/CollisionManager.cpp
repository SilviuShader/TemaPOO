#include "pch.h"
#include "CollisionManager.h"

using namespace std;

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::Update(list<shared_ptr<GameObject> >& gameObjects)
{
    for (shared_ptr<GameObject>& gameObject : gameObjects)
    {
        shared_ptr<Pseudo3DCamera> camera = gameObject->GetGame()->GetPseudo3DCamera();

        for (shared_ptr<GameObject>& other : gameObjects)
        {
            if (gameObject.get() != other.get())
            {
                shared_ptr<GameComponent> spriteRenderer1 = gameObject->GetComponent<SpriteRenderer>();
                shared_ptr<GameComponent> otherRenderer   = gameObject->GetComponent<SpriteRenderer>();

                if (spriteRenderer1 != nullptr && 
                    otherRenderer   != nullptr)
                {
                    float z      = gameObject->GetTransform()->GetPositionZ();
                    float otherZ = other->GetTransform()->GetPositionZ();

                    if (abs(z - otherZ) <= COLLISION_DISTANCE)
                    {
                        shared_ptr<SpriteRenderer> sprRend = dynamic_pointer_cast<SpriteRenderer>(spriteRenderer1);
                        shared_ptr<SpriteRenderer> othRend = dynamic_pointer_cast<SpriteRenderer>(otherRenderer);

                        RECT rect      = sprRend->GetSpriteRect();
                        RECT otherRect = othRend->GetSpriteRect();

                        float width      = (float)rect.right      * gameObject->GetTransform()->GetScale() * sprRend->GetSpriteScaleFactor();
                        float otherWidth = (float)otherRect.right * gameObject->GetTransform()->GetScale() * othRend->GetSpriteScaleFactor();

                        width      /= camera->GetWidth();
                        otherWidth /= camera->GetWidth();

                        float x      = gameObject->GetTransform()->GetPositionX();
                        float otherX = other->GetTransform()->GetPositionX();

                        float l1 = x - (width / 2.0f);
                        float r1 = x + (width / 2.0f);
                        float l2 = otherX - (otherWidth / 2.0f);
                        float r2 = otherX + (otherWidth / 2.0f);

                        // We might have a collision.
                        // Let's check it!
                        if ((l2 <= l1 && l1 <= r2) ||
                            (l2 <= r1 && r1 <= r2) ||
                            (l1 <= l2 && l2 <= r1) ||
                            (l1 <= r2 && r2 <= r1))
                            gameObject->OnCollisionUpdate(other);
                    }
                }
            }
        }
    }
}
