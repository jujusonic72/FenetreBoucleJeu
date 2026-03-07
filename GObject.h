#pragma once
#include "CClass.h"
#include "CProperty.h"

#define GENERATE_REFLECTION                          \
public:                                                                      \
    static CClass* StaticClass() { return &s_Class; }                        \
    virtual const CClass* GetClass() const override { return &s_Class; }     \
private:                                                                     \
    static CClass s_Class;


#define IMPLEMENT_REFLECTION(ClassName, ParentClass)                          \
    CClass ClassName::s_Class(#ClassName, ParentClass::StaticClass());



class GObject
{
public:
    virtual ~GObject();

    static CClass* StaticClass();
    virtual const CClass* GetClass() const;
    virtual void Start();
    virtual void Update(float dt);

private:
    static CClass s_Class;
};


template <class T>
T* Cast(GObject* InObject)
{
    if (InObject == nullptr) return nullptr;
    if (InObject->GetClass()->IsA(T::StaticClass()))
    {
        return static_cast<T*>(InObject);
    }
    return nullptr;
}
