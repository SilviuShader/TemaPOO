#pragma once

class CollisionManager
{
private:

    const float COLLISION_DISTANCE = 0.1f;

public:

    CollisionManager();
    ~CollisionManager();

    void Update(std::list<std::shared_ptr<GameObject> >&);
};