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

public:
    
    ObjectsGenerator(ContentManager*);
    ObjectsGenerator(const ObjectsGenerator&);
    ~ObjectsGenerator();

    void Update(std::list<std::shared_ptr<GameObject> >&, 
                Terrain*, 
                float, 
                float);

private:

    Zone                                               m_zone;
    float                                              m_accumulatedDistance;
    std::map<std::string, std::shared_ptr<Texture2D> > m_textures;
};