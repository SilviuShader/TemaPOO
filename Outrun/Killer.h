#pragma once

class Killer : public GameComponent
{
public:

    Killer(std::shared_ptr<GameObject>);
    ~Killer();

    void Update(float) override { };
    void Render()      override { };
};