#pragma once

#include "GObject.h"

class CEntity;

class CComponent : public GObject
{
    GENERATE_REFLECTION
public:
    CEntity* GetOwner() const         { return m_owner; }
    void     SetOwner(CEntity* owner) { m_owner = owner; }

private:
    CEntity* m_owner = nullptr;
};


