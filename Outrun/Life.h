#pragma once

class Life : public GameComponent
{
public:

    Life(std::shared_ptr<GameObject>);
    ~Life();

    inline void Update(float) override { };
    inline void Render()      override { };
};