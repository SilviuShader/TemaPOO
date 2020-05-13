#pragma once

class Life : public GameComponent
{
public:

    Life(GameObject*);
    ~Life();

    void Update(float) override { };
    void Render()      override { };
};