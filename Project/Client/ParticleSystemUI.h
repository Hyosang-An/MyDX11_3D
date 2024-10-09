#pragma once
#include "ComponentUI.h"
class ParticleSystemUI :
    public ComponentUI
{
    virtual void Update() override;

public:
    ParticleSystemUI();
    ~ParticleSystemUI();
};

