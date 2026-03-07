#include "CClass.h"

CClass::CClass(const char* name, CClass* parent)
: m_name(name)
, m_parent(parent)
{
}

const char* CClass::GetName() const
{
    return m_name;
}

CClass* CClass::GetParent() const
{
    return m_parent;
}

bool CClass::IsA(const CClass* other) const
{
    if (this == other) return true;
    if (m_parent != nullptr)
    {
        return m_parent->IsA(other);
    }
    return false;
}

void CClass::AddProperty(const char* name, EPropertyType type, size_t offset)
{
    m_properties.emplace_back(name, type, offset);
}

const std::vector<CProperty>& CClass::GetProperties() const
{
    return m_properties;
}

std::vector<CProperty> CClass::GetAllProperties() const
{
    std::vector<CProperty> all;

    if(m_parent != nullptr) all = m_parent->GetAllProperties(); // start by getting all properties in the parents

    for (const auto& prop : m_properties) all.push_back(prop); // then add own properties to the vector

    return all;
}
