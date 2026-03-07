#include "GObject.h"

// GObject est la racine, donc son parent est nullptr
CClass GObject::s_Class("GObject", nullptr);

GObject::~GObject()
{
}

CClass* GObject::StaticClass()
{
    return &s_Class;
}

const CClass* GObject::GetClass() const
{
    return &s_Class;
}

void GObject::Start()
{

}

void GObject::Update(float dt)
{

}