#include "CCounter.h"
#include "CGameEngine.h"

CCounter::CCounter() 
{
    CurrentTime = 0; 
    LastFrameCounter = SDL_GetPerformanceCounter();
}

CCounter::~CCounter()
{
    CurrentTime = 0;
    LastFrameCounter = 0;
}

void CCounter::UpdateCounter()
{
    Uint64 new_count = SDL_GetPerformanceCounter();
    Uint64 delta_ticks = new_count - LastFrameCounter;
    CurrentTime = float(double(delta_ticks)/double(SDL_GetPerformanceFrequency()));
    LastFrameCounter = new_count;
}

float CCounter::GetDeltaSeconds()
{
    return CurrentTime;
}
