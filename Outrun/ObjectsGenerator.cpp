#include "pch.h"
#include "ObjectsGenerator.h"

using namespace std;

ObjectsGenerator::ObjectsGenerator(shared_ptr<ContentManager> contentManager) :
    m_zone(ObjectsGenerator::Zone::Beach),
    m_accumulatedDistance(0.0f)
{
    shared_ptr<Texture2D> palmTree = contentManager->Load<Texture2D>("Palm.png");
    m_textures["Palm"] = palmTree;
}

ObjectsGenerator::~ObjectsGenerator()
{
    for (auto& texture : m_textures)
        texture.second.reset();
    m_textures.clear();
}

void ObjectsGenerator::Update(list<shared_ptr<GameObject> >& gameObjects, 
                              Terrain*                       terrain,
                              float                          playerSpeed, 
                              float                          deltaTime)
{
    m_accumulatedDistance += playerSpeed * deltaTime;

    if (m_accumulatedDistance > ACCUMULATE_TO_SPAWN)
    {
        string textureIndex = "";

        switch (m_zone)
        {
        case ObjectsGenerator::Zone::Beach:   
            textureIndex = "Palm";
            break;

        case ObjectsGenerator::Zone::City:
            break;

        case ObjectsGenerator::Zone::Mountains:
            break;
        }

        if (m_textures.find(textureIndex) != m_textures.end())
        {
            shared_ptr<GameObject> gameObject          = make_shared<GameObject>(terrain->GetParent()->GetGame());

            shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
            gameObject->AddComponent(objTranslator);

            gameObject->GetTransform()->SetPositionX((rand() % 2 ? 1.0f : -1.0f) * (terrain->GetParent()->GetGame()->GetRoadWidth() / 2.0f + 1.0f));

            shared_ptr<SpriteRenderer> spriteRenderer  = make_shared<SpriteRenderer>(gameObject,
                                                                                     m_textures[textureIndex]);
            gameObject->AddComponent(spriteRenderer);

            gameObjects.push_back(gameObject);

            m_accumulatedDistance -= ACCUMULATE_TO_SPAWN;
        }
    }
}
