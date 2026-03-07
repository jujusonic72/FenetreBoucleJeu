#include "CHealthComponent.h"
#include "CComponentRegistry.h"

IMPLEMENT_REFLECTION(CHealthComponent, CComponent)

REGISTER_COMPONENT(CHealthComponent)

REGISTER_PROPERTY(CHealthComponent, Health,    EPropertyType::Float)
REGISTER_PROPERTY(CHealthComponent, MaxHealth, EPropertyType::Float)
REGISTER_PROPERTY(CHealthComponent, IsAlive,   EPropertyType::Bool)
