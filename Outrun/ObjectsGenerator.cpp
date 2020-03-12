#include "pch.h"
#include "ObjectsGenerator.h"

using namespace std;

ObjectsGenerator::ObjectsGenerator(ContentManager* contentManager) :
    m_zone(ObjectsGenerator::Zone::Beach),
    m_accumulatedDistance(0.0f)
{
    shared_ptr<Texture2D> palmTree = contentManager->Load<Texture2D>("Palm.png");
    m_textures["Palm"] = palmTree;
}

ObjectsGenerator::ObjectsGenerator(const ObjectsGenerator& other)
{
}

ObjectsGenerator::~ObjectsGenerator()
{
}

void ObjectsGenerator::Update(list<shared_ptr<GameObject> >& gameObjects, 
                              Terrain*                       terrain,
                              float                          playerSpeed, 
                              float                          deltaTime)
{
    m_accumulatedDistance += playerSpeed * deltaTime;

    if (m_accumulatedDistance > ACCUMULATE_TO_SPAWN)
    {
        shared_ptr<GameObject> gameObject = make_shared<GameObject>(terrain->GetParent()->GetGame());

        shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject.get());
        gameObject->AddComponent(objTranslator);

        gameObject->GetTransform()->SetPositionX((rand() % 2 ? 1.0f : -1.0f) * (terrain->GetRoadWidth() / 2.0f + 1.0f));

        switch (m_zone)
        {
        case ObjectsGenerator::Zone::Beach:
        {
            shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject.get(),
                                                                                    m_textures["Palm"].get());
            gameObject->AddComponent(spriteRenderer);
        }   
        break;

        case ObjectsGenerator::Zone::City:
            break;

        case ObjectsGenerator::Zone::Mountains:
            break;
        }
        
        gameObjects.push_back(gameObject);

        m_accumulatedDistance -= ACCUMULATE_TO_SPAWN;
    }
}
