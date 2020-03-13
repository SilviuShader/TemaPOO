#pragma once

class ObjectTranslator : public GameComponent
{
public:

    ObjectTranslator(std::shared_ptr<GameObject>);
    ~ObjectTranslator();

           void Update(float) override;
    inline void Render()      override { };
};