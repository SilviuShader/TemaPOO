#pragma once

class Life : public GameComponent
{
public:

    Life(std::shared_ptr<GameObject>);
    ~Life();

    void Update(float) override { };
    void Render()      override { };
};