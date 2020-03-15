#pragma once

class Killer : public GameComponent
{
public:

    Killer(std::shared_ptr<GameObject>);
    ~Killer();

    inline void Update(float) override { };
    inline void Render()      override { };
};