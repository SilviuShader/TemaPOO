#pragma once

class Camera;
class Pseudo3DCamera;

class GameComponent
{
public:

    GameComponent();
    GameComponent(const GameComponent&);
    ~GameComponent();

    virtual void Update(float)           = 0;
    virtual void Render(Pseudo3DCamera*) = 0;
};