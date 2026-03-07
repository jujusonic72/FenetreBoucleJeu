#pragma once
#include "CProperty.h"
#include <vector>
class CClass
{
public:
    CClass(const char* name, CClass* parent);
    const char* GetName() const;
    CClass* GetParent() const;
    bool IsA(const CClass* other) const;

    //Properties
    void AddProperty(const char* name, EPropertyType type, size_t offset); // registers a property in this class. called manually to declare members to expose to the reflexion
    const std::vector<CProperty>& GetProperties() const; // returns all properties in this class
    std::vector<CProperty> GetAllProperties() const; // returns all properties including the ones in the parent classes

private:
    const char* m_name;
    CClass* m_parent;
    std::vector<CProperty> m_properties;
};
