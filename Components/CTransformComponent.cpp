#include "CTransformComponent.h"
#include "CComponentRegistry.h"

IMPLEMENT_REFLECTION(CTransformComponent, CComponent)

REGISTER_COMPONENT(CTransformComponent)

REGISTER_PROPERTY(CTransformComponent, PosX,     EPropertyType::Float)
REGISTER_PROPERTY(CTransformComponent, PosY,     EPropertyType::Float)
REGISTER_PROPERTY(CTransformComponent, PosZ,     EPropertyType::Float)
REGISTER_PROPERTY(CTransformComponent, Rotation, EPropertyType::Float)
