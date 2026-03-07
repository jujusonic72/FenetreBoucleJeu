#pragma once

#include "GObject.h"
#include <vector>
#include <string>

class CComponent;

class CEntity : public GObject
{
    GENERATE_REFLECTION
public:
    char Name[64] = "Entity";

    void AddComponent(CComponent* comp);
    void RemoveComponent(CComponent* comp);

    const std::vector<CComponent*>& GetComponents() const;

    // Propage Update(dt) à tous les composants
    void Update(float dt) override;

    template<class T>
    T* GetComponent() const
    {
        for (CComponent* comp : m_components)
        {
            T* result = Cast<T>(comp);
            if (result) return result;
        }
        return nullptr;
    }

private:
    std::vector<CComponent*> m_components;
};