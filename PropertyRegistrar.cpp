#include "CProperty.h"
#include "CClass.h"

PropertyRegistrar::PropertyRegistrar(CClass* cls, const char* name, EPropertyType type, size_t offset)
{
    if (cls)
        cls->AddProperty(name, type, offset);
}
