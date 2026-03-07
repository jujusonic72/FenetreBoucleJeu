#include "CEntity.h"
#include "CComponent.h"
#include <algorithm>
#include <cstring>

IMPLEMENT_REFLECTION(CEntity, GObject)

void CEntity::AddComponent(CComponent* comp)
{
    if (comp)
    {
        comp->SetOwner(this);
        m_components.push_back(comp);
    }
}

void CEntity::RemoveComponent(CComponent* comp)
{
    m_components.erase(
        std::remove(m_components.begin(), m_components.end(), comp),
        m_components.end());
}

const std::vector<CComponent*>& CEntity::GetComponents() const
{
    return m_components;
}

void CEntity::Update(float dt)
{
    for (CComponent* comp : m_components)
        comp->Update(dt);
}