#pragma once
#include <SDL3/SDL_timer.h>

class CCounter
{
public:
    CCounter();
    ~CCounter();
    void UpdateCounter();
    float GetDeltaSeconds();
private:
    float CurrentTime;
    Uint64 LastFrameCounter;
    
}; 
