#pragma once

class Terrain;

class ObjectsGenerator
{
private:

    enum class Zone
    {
        Beach,
        City,
        Mountains,
        Last
    };

private:

    const float ACCUMULATE_TO_SPAWN        = 1.0f;
    const float ACCUMULATE_TO_CHANGE_ZONE  = 10.0f;
    const float BORDER_ACCUMULATE_TO_SPAWN = 0.5f;
    const float MAX_OBJECT_DISPLACEMENT    = 3.0f;
    const float CAR_CHANCE                 = 0.5f;
    const float MIN_CAR_SPEED              = 0.5f;
    const float MAX_CAR_SPEED              = 4.0f;

public:
    
    ObjectsGenerator(std::shared_ptr<ContentManager>);
    ~ObjectsGenerator();

    void Update(std::shared_ptr<Game>, 
                float);

private:

    void BorderSpawnUpdate(std::shared_ptr<Game>,
                           float);
    void ZoneSpawnUpdate(std::shared_ptr<Game>,
                         float);
    void CarSpawnUpdate(std::shared_ptr<Game>, 
                        float);

private:

    Zone                                               m_zone;
    bool                                               m_lastBorder;
    float                                              m_borderAccumulatedDistance;
    float                                              m_accumulatedDistance;
    float                                              m_accumulatedZone;
    float                                              m_accumulatedCarChance;
    std::map<std::string, std::shared_ptr<Texture2D> > m_textures;
};