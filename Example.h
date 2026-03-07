#pragma once
#include "GObject.h"

class GPlayerState : public GObject
{
    GENERATE_REFLECTION
public:
    int _Points = 0;
};


class GAnimal : public GObject
{
    GENERATE_REFLECTION
public:
    const char* Name = "Unknown";
};

class GCat : public GAnimal
{
    GENERATE_REFLECTION
public:
    int Lives = 9;
};

class GDog : public GAnimal
{
    GENERATE_REFLECTION
public:
    bool GoodBoy = true;
};
