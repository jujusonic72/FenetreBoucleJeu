#pragma once
#include <string>

enum class EPropertyType
{
    Int,
    Float,
    Bool,
    String // const char*
};

struct CProperty
{
    CProperty(const char* name, EPropertyType type, size_t offset)
        : Name(name)
        , Type(type)
        , Offset(offset)
    {
    }

    const char* Name; // name to display
    EPropertyType Type; // to determine which imgui widget to use
    size_t Offset; // position in bytes from the start of the object in memory

    void* GetValuePtr(void* obj) const
    {
        return (char*)obj + Offset; // return the pointer to the member's adress using the object and the offset
    }

    const void* GetValuePtr(const void* obj) const // the const version
    {
        return (const char*)obj + Offset;
    }
};

// ---------------------------------------------------------------------------
// PropertyRegistrar
// Permet d'appeler AddProperty() au scope global (hors de toute fonction)
// via l'initialisation statique — même pattern que ComponentRegistrar.
// L'ordre dans un même .cpp est garanti top-to-bottom, donc
// IMPLEMENT_REFLECTION doit toujours précéder REGISTER_PROPERTY.
// ---------------------------------------------------------------------------
class CClass;

struct PropertyRegistrar
{
    PropertyRegistrar(CClass* cls, const char* name, EPropertyType type, size_t offset);
};

/*
 * REGISTER_PROPERTY(ClassName, PropertyName, PropertyType)
 * À placer dans le .cpp du composant, après IMPLEMENT_REFLECTION.
 *
 * Exemple :
 *   IMPLEMENT_REFLECTION(CHealthComponent, CComponent)
 *   REGISTER_PROPERTY(CHealthComponent, Health,    EPropertyType::Float)
 *   REGISTER_PROPERTY(CHealthComponent, MaxHealth, EPropertyType::Float)
 */
#define REGISTER_PROPERTY(ClassName, PropertyName, PropertyType)            \
    static PropertyRegistrar _propReg_##ClassName##_##PropertyName(         \
        ClassName::StaticClass(),                                            \
        #PropertyName,                                                       \
        PropertyType,                                                        \
        offsetof(ClassName, PropertyName));

