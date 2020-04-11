#pragma once

class Transform : public GameComponent
{
public:
    
    Transform(std::shared_ptr<GameObject>);
    Transform(std::shared_ptr<GameObject>, 
              float, 
              float);
    ~Transform();

           void  Update(float)           override;
           void  Render()                override {                          }

    inline void  SetPositionZ(float positionZ)    { m_positionZ = positionZ; }
    inline void  SetPositionX(float positionX)    { m_positionX = positionX; }

    inline float GetPositionX()          const    { return m_positionX;      }
    inline float GetPositionZ()          const    { return m_positionZ;      }
    inline float GetScale()              const    { return m_scale;          }

private:

    float m_positionX;
    float m_positionZ;
    float m_scale;
};