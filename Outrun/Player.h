#pragma once

class Player : public GameComponent
{
private:

    const float MAX_SPEED                   = 5.0f;
    const float MAX_STEER_SPEED             = 1.0f;
    const float STEER_ACCELERATION_MULTIP   = 5.0f;
    const float STEER_ACCELERATION_DECREASE = 2.5f;
    const float SPEED_ACCELERATION_MULTIP   = 1.0f;
    const float BRAKE_MULTIP                = 2.5f;
    const float ACCIDENT_BRAKE              = 1.0f;

    const int   MAX_LIVES_COUNT             = 3;

public:

    Player(GameObject*);
    ~Player();

           void    Update(float)                                  override;
           void    Render()                                       override {                     }
           void    OnCollisionUpdate(std::shared_ptr<GameObject>) override;

           Player& operator++();
           Player& operator--();

           Player& operator++(int);
           Player& operator--(int);

           Player& operator+=(const int&);

    inline float   GetPositionX()                                 const    { return m_positionX;  }
    inline float   GetMaxSpeed()                                  const    { return MAX_SPEED;    }
    inline float   GetSpeed()                                     const    { return m_speed;      }
    inline float   GetDistance()                                  const    { return m_distance;   }

    inline int     GetLivesCount()                                const    { return m_livesCount; }

private:

    std::shared_ptr<SpriteRenderer> m_spriteRenderer;
    float                           m_positionX;
    float                           m_speed;
    float                           m_steerSpeed;
    float                           m_distance;
    int                             m_livesCount;
};