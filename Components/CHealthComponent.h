#include "CComponent.h"

class CHealthComponent : public CComponent
{
    GENERATE_REFLECTION
public:
    float Health    = 100.0f;
    float MaxHealth = 100.0f;
    bool  IsAlive   = true;
};