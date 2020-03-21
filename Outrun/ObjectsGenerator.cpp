#include "pch.h"
#include "ObjectsGenerator.h"

using namespace std;

ObjectsGenerator::ObjectsGenerator(shared_ptr<ContentManager> contentManager) :
    m_zone(ObjectsGenerator::Zone::Beach),
    m_lastBorder(false),
    m_borderAccumulatedDistance(0.0f),
    m_accumulatedDistance(0.0f),
    m_accumulatedZone(0.0f),
    m_accumulatedCarChance(0.0f),
    m_accumulatedMotorChance(0.0f),
    m_accumulatedLifeChance(0.0f)
{
    m_prevZone = m_zone;

    shared_ptr<Texture2D> border = contentManager->Load<Texture2D>("Border.png");
    m_textures["Border"] = border;

    shared_ptr<Texture2D> palmTree = contentManager->Load<Texture2D>("Palm.png");
    m_textures["Palm"] = palmTree;

    shared_ptr<Texture2D> tower = contentManager->Load<Texture2D>("Tower.png");
    m_textures["Tower"] = tower;

    shared_ptr<Texture2D> mountains = contentManager->Load<Texture2D>("Mountains.png");
    m_textures["Mountains"] = mountains;

    shared_ptr<Texture2D> car = contentManager->Load<Texture2D>("CarBack.png");
    m_textures["CarBack"] = car;

    car = contentManager->Load<Texture2D>("CarFront.png");
    m_textures["CarFront"] = car;

    shared_ptr<Texture2D> motor = contentManager->Load<Texture2D>("MotorBack.png");
    m_textures["MotorBack"] = motor;

    shared_ptr<Texture2D> life = contentManager->Load<Texture2D>("Life.png");
    m_textures["Life"] = life;
}

ObjectsGenerator::~ObjectsGenerator()
{
    for (const auto& [k, v] : m_textures)
        m_textures[k].reset();

    m_textures.clear();
}

void ObjectsGenerator::Update(shared_ptr<Game> game,
                              float            deltaTime)
{
    BorderSpawnUpdate(game, deltaTime);
    ZoneSpawnUpdate(game, deltaTime);
    CarSpawnUpdate(game, deltaTime);
    MotorSpawnUpdate(game, deltaTime);
    LifeSpawnUpdate(game, deltaTime);
}

void ObjectsGenerator::BorderSpawnUpdate(shared_ptr<Game> game, float deltaTime)
{
    float                          playerSpeed = game->GetPlayer()->GetSpeed();
    list<shared_ptr<GameObject> >& gameObjects = game->GetGameObjects();

    m_borderAccumulatedDistance += playerSpeed * deltaTime;

    if (m_borderAccumulatedDistance >= BORDER_ACCUMULATE_TO_SPAWN)
    {
        shared_ptr<GameObject> gameObject = make_shared<GameObject>(game);
        shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
        gameObject->AddComponent(objTranslator);

        gameObject->GetTransform()->SetPositionX((m_lastBorder ? 1.0f : -1.0f) * (game->GetRoadWidth() + 0.05f));

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject,
            m_textures["Border"]);
        gameObject->AddComponent(spriteRenderer);

        gameObjects.push_back(gameObject);

        m_borderAccumulatedDistance -= BORDER_ACCUMULATE_TO_SPAWN;
        m_lastBorder = !m_lastBorder;
    }
}

void ObjectsGenerator::ZoneSpawnUpdate(shared_ptr<Game> game, 
                                       float            deltaTime)
{
    float                          playerSpeed = game->GetPlayer()->GetSpeed();
    list<shared_ptr<GameObject> >& gameObjects = game->GetGameObjects();

    m_accumulatedDistance += playerSpeed * deltaTime;
    m_accumulatedZone += playerSpeed * deltaTime;

    if (m_accumulatedZone >= ACCUMULATE_TO_CHANGE_ZONE)
    {
        m_prevZone = m_zone;
        m_zone = (ObjectsGenerator::Zone)(rand() % ((int)ObjectsGenerator::Zone::Last));
        if (m_zone == m_prevZone)
            m_zone = (ObjectsGenerator::Zone)(((int)m_zone + 1) % (int)ObjectsGenerator::Zone::Last);

        m_accumulatedZone -= ACCUMULATE_TO_CHANGE_ZONE;
    }

    if (m_accumulatedDistance > ACCUMULATE_TO_SPAWN)
    {
        string textureIndex = "";

        float prevChance = (m_accumulatedZone / ACCUMULATE_TO_CHANGE_ZONE);
        
        switch (Zone crtZone = ((Utils::RandomFloat() > prevChance) ? m_prevZone : m_zone); crtZone)
        {
        case ObjectsGenerator::Zone::Beach:
            textureIndex = "Palm";
            break;

        case ObjectsGenerator::Zone::City:
            textureIndex = "Tower";
            break;

        case ObjectsGenerator::Zone::Mountains:
            textureIndex = "Mountains";
            break;
        }

        if (m_textures.find(textureIndex) != m_textures.end())
        {
            shared_ptr<GameObject> gameObject = make_shared<GameObject>(game);
            shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
            gameObject->AddComponent(objTranslator);

            float displacement = Utils::RandomFloat() * MAX_OBJECT_DISPLACEMENT;

            gameObject->GetTransform()->SetPositionX((rand() % 2 ? 1.0f : -1.0f) * (game->GetRoadWidth() / 2.0f + 3.0f + displacement));

            shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject,
                m_textures[textureIndex]);
            gameObject->AddComponent(spriteRenderer);

            shared_ptr<Killer> killer = make_shared<Killer>(gameObject);
            gameObject->AddComponent(killer);

            gameObjects.push_back(gameObject);

            m_accumulatedDistance -= ACCUMULATE_TO_SPAWN;
        }
    }
}

void ObjectsGenerator::CarSpawnUpdate(shared_ptr<Game> game,
                                      float            deltaTime)
{
    list<shared_ptr<GameObject> >& gameObjects = game->GetGameObjects();

    m_accumulatedCarChance += deltaTime * Utils::RandomFloat();

    if (m_accumulatedCarChance >= CAR_CHANCE)
    {
        bool goingForward = rand() % 2;

        shared_ptr<GameObject> gameObject = make_shared<GameObject>(game);

        shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
        objTranslator->SetObjectSpeed(Utils::Lerp(Utils::RandomFloat(),
                                                  MIN_CAR_SPEED, 
                                                  MAX_CAR_SPEED) * 
                                      (goingForward ? -1.0f : 1.0f));

        gameObject->AddComponent(objTranslator);

        gameObject->GetTransform()->SetPositionX((goingForward ? 1.0f : -1.0f) * (game->GetRoadWidth() / 2.0f));

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject,
                                                                                m_textures[(goingForward ? "CarBack" : "CarFront")]);
        gameObject->AddComponent(spriteRenderer);

        shared_ptr<Killer> killer = make_shared<Killer>(gameObject);
        gameObject->AddComponent(killer);

        gameObjects.push_back(gameObject);

        m_accumulatedCarChance -= CAR_CHANCE;
    }
}

void ObjectsGenerator::MotorSpawnUpdate(shared_ptr<Game> game, 
                                        float            deltaTime)
{
    list<shared_ptr<GameObject> >& gameObjects = game->GetGameObjects();

    m_accumulatedMotorChance += deltaTime * Utils::RandomFloat();
    if (m_accumulatedMotorChance >= MOTOR_CHANCE)
    {
        shared_ptr<GameObject> gameObject = make_shared<GameObject>(game);

        shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
        objTranslator->SetObjectSpeed(Utils::Lerp(Utils::RandomFloat(),
                                                  MIN_CAR_SPEED,
                                                  MAX_CAR_SPEED) * -1.0f);

        gameObject->AddComponent(objTranslator);

        gameObject->GetTransform()->SetPositionX(0.0f);

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject,
            m_textures["MotorBack"]);
        gameObject->AddComponent(spriteRenderer);

        shared_ptr<Killer> killer = make_shared<Killer>(gameObject);
        gameObject->AddComponent(killer);

        gameObjects.push_back(gameObject);

        m_accumulatedMotorChance -= MOTOR_CHANCE;
    }
}

void ObjectsGenerator::LifeSpawnUpdate(shared_ptr<Game> game, 
                                       float            deltaTime)
{
    list<shared_ptr<GameObject> >& gameObjects = game->GetGameObjects();

    m_accumulatedLifeChance += deltaTime * Utils::RandomFloat();
    if (m_accumulatedLifeChance >= LIFE_CHANCE)
    {
        shared_ptr<GameObject> gameObject = make_shared<GameObject>(game);
        shared_ptr<ObjectTranslator> objTranslator = make_shared<ObjectTranslator>(gameObject);
        gameObject->AddComponent(objTranslator);
        gameObject->GetTransform()->SetPositionX((rand() % 2 ? 1.0f : -1.0f) * ((game->GetRoadWidth() / 2.0f) * Utils::RandomFloat()));

        shared_ptr<SpriteRenderer> spriteRenderer = make_shared<SpriteRenderer>(gameObject,
                                                                                m_textures["Life"]);
        gameObject->AddComponent(spriteRenderer);

        shared_ptr<Life> life = make_shared<Life>(gameObject);
        gameObject->AddComponent(life);

        gameObjects.push_back(gameObject);

        m_accumulatedLifeChance -= LIFE_CHANCE;
    }
}
