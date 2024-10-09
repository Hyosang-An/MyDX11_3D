#pragma once
#include "AssetUI.h"
class FlipBookUI :
    public AssetUI
{
public:
    virtual void Update() override;

public:
    FlipBookUI();
    ~FlipBookUI();
};

