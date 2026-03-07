#include "CComponent.h"

class CTransformComponent : public CComponent
{
    GENERATE_REFLECTION
public:
    float PosX     = 0.0f;
    float PosY     = 0.0f;
    float PosZ     = 0.0f;
    float Rotation = 0.0f;
};