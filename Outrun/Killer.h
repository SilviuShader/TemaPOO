#pragma once

class Killer : public GameComponent
{
public:

    Killer(GameObject*);
    ~Killer();

    void Update(float) override { };
    void Render()      override { };
};