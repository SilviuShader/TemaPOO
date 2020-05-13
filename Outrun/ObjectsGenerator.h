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

    const float ACCUMULATE_TO_SPAWN        = 0.75f;
    const float ACCUMULATE_TO_CHANGE_ZONE  = 50.0f;
    const float BORDER_ACCUMULATE_TO_SPAWN = 0.5f;
    const float MAX_OBJECT_DISPLACEMENT    = 3.0f;
    const float CAR_CHANCE                 = 1.0f;
    const float MOTOR_CHANCE               = 2.0f;
    const float LIFE_CHANCE                = 4.0f;
    const float MIN_CAR_SPEED              = 0.5f;
    const float MAX_CAR_SPEED              = 4.0f;

public:
    
    ObjectsGenerator(std::shared_ptr<Texture2DManager>);
    ~ObjectsGenerator();

    void Update(Game*, 
                float);

private:

    void BorderSpawnUpdate(Game*,
                           float);
    void ZoneSpawnUpdate(Game*,
                         float);
    void CarSpawnUpdate(Game*, 
                        float);
    void MotorSpawnUpdate(Game*,
                          float);
    void LifeSpawnUpdate(Game*,
                         float);

private:

    Zone                                               m_zone;
    Zone                                               m_prevZone;
    bool                                               m_lastBorder;
    float                                              m_borderAccumulatedDistance;
    float                                              m_accumulatedDistance;
    float                                              m_accumulatedZone;
    float                                              m_accumulatedCarChance;
    float                                              m_accumulatedMotorChance;
    float                                              m_accumulatedLifeChance;
    std::map<std::string, std::shared_ptr<Texture2D> > m_textures;
};