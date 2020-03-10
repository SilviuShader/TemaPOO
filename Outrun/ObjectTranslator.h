#pragma once

class ObjectTranslator : public GameComponent
{
public:

    ObjectTranslator(GameObject*);
    ~ObjectTranslator();

    void Update(float)           override;
    void Render(Pseudo3DCamera*) override;

private:

    float m_translation;
};