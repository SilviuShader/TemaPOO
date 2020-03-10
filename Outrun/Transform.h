#pragma once

class Transform : public GameComponent
{
public:
    
    Transform(GameObject*);
    Transform(GameObject*, 
              int, 
              float);
    ~Transform();

    void  Update(float)                  override;
    void  Render(Pseudo3DCamera* camera) override {                          }

    inline void  SetLine(int line)                { m_screenLine = line;     }
    inline void  SetPositionX(float positionX)    { m_positionX = positionX; }

    inline int   GetLine()               const    { return m_screenLine;     }
    inline float GetPositionX()          const    { return m_positionX;      }
    inline float GetScale()              const    { return m_scale;          }

private:

    int   m_screenLine;
    float m_positionX;
    float m_scale;
};