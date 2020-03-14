#pragma once

class Terrain;

class ObjectsGenerator
{
private:

    enum class Zone
    {
        Beach,
        City,
        Mountains
    };

private:

    const float ACCUMULATE_TO_SPAWN = 2.0f;
    const float CAR_CHANCE          = 0.5f;
    const float MIN_CAR_SPEED       = 0.5f;
    const float MAX_CAR_SPEED       = 4.0f;

public:
    
    ObjectsGenerator(std::shared_ptr<ContentManager>);
    ~ObjectsGenerator();

    void Update(std::shared_ptr<Game>, 
                float);

private:

    void CarSpawnUpdate(std::shared_ptr<Game>, 
                        float);

private:

    Zone                                               m_zone;
    float                                              m_accumulatedDistance;
    float                                              m_accumulatedCarChance;
    std::map<std::string, std::shared_ptr<Texture2D> > m_textures;
};