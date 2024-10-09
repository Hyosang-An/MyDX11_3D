#pragma once
#include "ComponentUI.h"
class Light2DUI :
    public ComponentUI
{
public:
    virtual void Update() override;

public:
    Light2DUI();
    ~Light2DUI();
};

