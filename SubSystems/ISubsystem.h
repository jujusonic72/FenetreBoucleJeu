#pragma once

class ISubsystem
{
public:
    virtual ~ISubsystem() {}
    virtual bool Start()=0;
    virtual void Update(float deltaSeconds)=0;
    virtual void Shutdown()=0;
};
